package com.github.thejunkjon.androidintrospection.vpn

import android.app.PendingIntent
import android.content.pm.PackageManager
import android.net.ProxyInfo
import android.net.VpnService
import android.os.ParcelFileDescriptor
import android.text.TextUtils
import timber.log.Timber
import java.io.FileInputStream
import java.io.FileOutputStream
import java.io.IOException
import java.net.InetSocketAddress
import java.net.SocketAddress
import java.net.SocketException
import java.nio.ByteBuffer
import java.nio.channels.DatagramChannel
import java.nio.charset.StandardCharsets
import java.util.concurrent.TimeUnit

class SimpleVpnConnection(
    private val service: VpnService,
    private val connectionId: Int,
    private val serverName: String,
    private val serverPort: Int,
    private val sharedSecret: ByteArray,
    proxyHostName: String?,
    proxyHostPort: Int,
    allow: Boolean,
    packages: Set<String>
) : Runnable {

    interface OnEstablishListener {
        fun onEstablish(tunInterface: ParcelFileDescriptor?)
    }

    private var configureIntent: PendingIntent? = null
    private var onEstablishListener: OnEstablishListener? = null

    private var proxyHostName: String? = null
    private var proxyHostPort = 0

    private val allow: Boolean
    private val packages: Set<String>

    fun setConfigureIntent(intent: PendingIntent?) {
        configureIntent = intent
    }

    fun setOnEstablishListener(listener: OnEstablishListener?) {
        onEstablishListener = listener
    }

    override fun run() {
        try {
            Timber.i("Starting")
            // If anything needs to be obtained using the network, get it now.
            // This greatly reduces the complexity of seamless handover, which
            // tries to recreate the tunnel without shutting down everything.
            // In this demo, all we need to know is the server address.
            val serverAddress: SocketAddress = InetSocketAddress(serverName, serverPort)
            // We try to create the tunnel several times.
            // TODO: The better way is to work with ConnectivityManager, trying only when the
            // network is available.
            // Here we just use a counter to keep things simple.
            var attempt = 0
            while (attempt < 10) {

                // Reset the counter if we were connected.
                if (run(serverAddress)) {
                    attempt = 0
                }
                // Sleep for a while. This also checks if we got interrupted.
                Thread.sleep(3000)
                ++attempt
            }
            Timber.i("Giving up")
        } catch (e: IOException) {
            Timber.e(e, "Connection failed, exiting")
        } catch (e: InterruptedException) {
            Timber.e(e, "Connection failed, exiting")
        } catch (e: IllegalArgumentException) {
            Timber.e(e, "Connection failed, exiting")
        }
    }

    @Throws(IOException::class, InterruptedException::class, IllegalArgumentException::class)
    private fun run(server: SocketAddress): Boolean {
        var iface: ParcelFileDescriptor? = null
        var connected = false
        // Create a DatagramChannel as the VPN tunnel.
        try {
            DatagramChannel.open().use { tunnel ->
                // Protect the tunnel before connecting to avoid loopback.
                check(service.protect(tunnel.socket())) { "Cannot protect the tunnel" }
                // Connect to the server.
                tunnel.connect(server)
                // For simplicity, we use the same thread for both reading and
                // writing. Here we put the tunnel into non-blocking mode.
                tunnel.configureBlocking(false)
                // Authenticate and configure the virtual network interface.
                iface = handshake(tunnel)
                // Now we are connected. Set the flag.
                connected = true
                // Packets to be sent are queued in this input stream.
                val `in` = FileInputStream(iface!!.fileDescriptor)
                // Packets received need to be written to this output stream.
                val out = FileOutputStream(iface!!.fileDescriptor)
                // Allocate the buffer for a single packet.
                val packet = ByteBuffer.allocate(MAX_PACKET_SIZE)
                // Timeouts:
                //   - when data has not been sent in a while, send empty keepalive messages.
                //   - when data has not been received in a while, assume the connection is broken.
                var lastSendTime = System.currentTimeMillis()
                var lastReceiveTime = System.currentTimeMillis()
                // We keep forwarding packets till something goes wrong.
                while (true) {
                    // Assume that we did not make any progress in this iteration.
                    var idle = true
                    // Read the outgoing packet from the input stream.
                    var length = `in`.read(packet.array())
                    if (length > 0) {
                        // Write the outgoing packet to the tunnel.
                        packet.limit(length)
                        tunnel.write(packet)
                        packet.clear()
                        // There might be more outgoing packets.
                        idle = false
                        lastReceiveTime = System.currentTimeMillis()
                    }
                    // Read the incoming packet from the tunnel.
                    length = tunnel.read(packet)
                    if (length > 0) {
                        // Ignore control messages, which start with zero.
                        if (packet[0].toInt() != 0) {
                            // Write the incoming packet to the output stream.
                            out.write(packet.array(), 0, length)
                        }
                        packet.clear()
                        // There might be more incoming packets.
                        idle = false
                        lastSendTime = System.currentTimeMillis()
                    }
                    // If we are idle or waiting for the network, sleep for a
                    // fraction of time to avoid busy looping.
                    if (idle) {
                        Thread.sleep(IDLE_INTERVAL_MS)
                        val timeNow = System.currentTimeMillis()
                        if (lastSendTime + KEEPALIVE_INTERVAL_MS <= timeNow) {
                            // We are receiving for a long time but not sending.
                            // Send empty control messages.
                            packet.put(0.toByte()).limit(1)
                            for (i in 0..2) {
                                packet.position(0)
                                tunnel.write(packet)
                            }
                            packet.clear()
                            lastSendTime = timeNow
                        } else check(lastReceiveTime + RECEIVE_TIMEOUT_MS > timeNow) {
                            // We are sending for a long time but not receiving.
                            "Timed out"
                        }
                    }
                }
            }
        } catch (e: SocketException) {
            Timber.e(e, "Cannot use socket")
        } finally {
            if (iface != null) {
                try {
                    iface!!.close()
                } catch (e: IOException) {
                    Timber.e(e, "Unable to close interface")
                }
            }
        }
        return connected
    }

    @Throws(IOException::class, InterruptedException::class)
    private fun handshake(tunnel: DatagramChannel): ParcelFileDescriptor {
        // To build a secured tunnel, we should perform mutual authentication
        // and exchange session keys for encryption. To keep things simple in
        // this demo, we just send the shared secret in plaintext and wait
        // for the server to send the parameters.
        // Allocate the buffer for handshaking. We have a hardcoded maximum
        // handshake size of 1024 bytes, which should be enough for demo
        // purposes.
        val packet = ByteBuffer.allocate(1024)
        // Control messages always start with zero.
        packet.put(0.toByte()).put(sharedSecret).flip()
        // Send the secret several times in case of packet loss.
        for (i in 0..2) {
            packet.position(0)
            tunnel.write(packet)
        }
        packet.clear()
        // Wait for the parameters within a limited time.
        for (i in 0 until MAX_HANDSHAKE_ATTEMPTS) {
            Thread.sleep(IDLE_INTERVAL_MS)
            // Normally we should not receive random packets. Check that the first
            // byte is 0 as expected.
            val length = tunnel.read(packet)
            if (length > 0 && packet[0].toInt() == 0) {
                return configure(
                    String(
                        packet.array(),
                        1,
                        length - 1,
                        StandardCharsets.US_ASCII
                    ).trim { it <= ' ' })
            }
        }
        throw IOException("Timed out")
    }

    @Throws(IllegalArgumentException::class)
    private fun configure(parameters: String): ParcelFileDescriptor {
        // Configure a builder while parsing the parameters.
        val builder = service.Builder()
        for (parameter in parameters.split(" ".toRegex()).toTypedArray()) {
            val fields = parameter.split(",".toRegex()).toTypedArray()
            try {
                when (fields[0][0]) {
                    'm' -> builder.setMtu(fields[1].toShort().toInt())
                    'a' -> builder.addAddress(fields[1], fields[2].toInt())
                    'r' -> builder.addRoute(fields[1], fields[2].toInt())
                    'd' -> builder.addDnsServer(fields[1])
                    's' -> builder.addSearchDomain(fields[1])
                }
            } catch (e: NumberFormatException) {
                throw IllegalArgumentException("Bad parameter: $parameter")
            }
        }
        // Create a new interface using the builder and save the parameters.
        var vpnInterface: ParcelFileDescriptor
        for (packageName in packages) {
            try {
                if (allow) {
                    builder.addAllowedApplication(packageName)
                } else {
                    builder.addDisallowedApplication(packageName)
                }
            } catch (e: PackageManager.NameNotFoundException) {
                Timber.w(e, "Package not available: $packageName")
            }
        }
        builder.setSession(serverName).setConfigureIntent(configureIntent)
        if (!TextUtils.isEmpty(proxyHostName)) {
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.Q) {
                builder.setHttpProxy(ProxyInfo.buildDirectProxy(proxyHostName, proxyHostPort))
            }
        }
        synchronized(service) {
            vpnInterface = builder.establish()
            if (onEstablishListener != null) {
                onEstablishListener!!.onEstablish(vpnInterface)
            }
        }
        Timber.i("New interface: $vpnInterface ($parameters)")
        return vpnInterface
    }

    companion object {
        /** Maximum packet size is constrained by the MTU, which is given as a signed short.  */
        private const val MAX_PACKET_SIZE = Short.MAX_VALUE.toInt()

        /** Time to wait in between losing the connection and retrying.  */
        private val RECONNECT_WAIT_MS = TimeUnit.SECONDS.toMillis(3)

        /** Time between keepalives if there is no traffic at the moment.
         *
         * TODO: don't do this; it's much better to let the connection die and then reconnect when
         * necessary instead of keeping the network hardware up for hours on end in between.
         */
        private val KEEPALIVE_INTERVAL_MS = TimeUnit.SECONDS.toMillis(15)

        /** Time to wait without receiving any response before assuming the server is gone.  */
        private val RECEIVE_TIMEOUT_MS = TimeUnit.SECONDS.toMillis(20)

        /**
         * Time between polling the VPN interface for new traffic, since it's non-blocking.
         *
         * TODO: really don't do this; a blocking read on another thread is much cleaner.
         */
        private val IDLE_INTERVAL_MS = TimeUnit.MILLISECONDS.toMillis(100)

        /**
         * Number of periods of length {@IDLE_INTERVAL_MS} to wait before declaring the handshake a
         * complete and abject failure.
         *
         * TODO: use a higher-level protocol; hand-rolling is a fun but pointless exercise.
         */
        private const val MAX_HANDSHAKE_ATTEMPTS = 50
    }

    init {
        if (!TextUtils.isEmpty(proxyHostName)) {
            this.proxyHostName = proxyHostName
        }
        if (proxyHostPort > 0) {
            // The port value is always an integer due to the configured inputType.
            this.proxyHostPort = proxyHostPort
        }
        this.allow = allow
        this.packages = packages
    }
}