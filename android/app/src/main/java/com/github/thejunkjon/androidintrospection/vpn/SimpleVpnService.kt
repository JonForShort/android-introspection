package com.github.thejunkjon.androidintrospection.vpn

import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Intent
import android.net.VpnService
import android.os.Handler
import android.os.Message
import android.os.ParcelFileDescriptor
import android.util.Pair
import android.widget.Toast
import com.github.thejunkjon.androidintrospection.R
import com.github.thejunkjon.androidintrospection.vpn.SimpleVpnClient.SettingsPreference
import timber.log.Timber
import java.io.IOException
import java.util.concurrent.atomic.AtomicInteger
import java.util.concurrent.atomic.AtomicReference

class SimpleVpnService : VpnService(), Handler.Callback {

    companion object {
        internal const val ACTION_CONNECT = "com.github.thejunkjon.androidintrospection.vpn.START"
        internal const val ACTION_DISCONNECT = "com.github.thejunkjon.androidintrospection.vpn.STOP"
        internal const val NOTIFICATION_CHANNEL_ID = "SimpleVpn"
    }

    private class Connection(thread: Thread, descriptor: ParcelFileDescriptor) : Pair<Thread, ParcelFileDescriptor>(thread, descriptor)

    private var handler: Handler? = null
    private val connectingThread = AtomicReference<Thread?>()
    private val connection = AtomicReference<Connection?>()
    private val nextConnectionId = AtomicInteger(1)
    private var configureIntent: PendingIntent? = null

    override fun onCreate() {
        if (handler == null) {
            handler = Handler(this)
        }
        configureIntent = PendingIntent.getActivity(this, 0, Intent(this, SimpleVpnClient::class.java), PendingIntent.FLAG_UPDATE_CURRENT)
    }

    override fun onStartCommand(intent: Intent, flags: Int, startId: Int): Int {
        return if (ACTION_DISCONNECT == intent.action) {
            disconnect()
            START_NOT_STICKY
        } else {
            connect()
            START_STICKY
        }
    }

    override fun onDestroy() {
        disconnect()
    }

    override fun handleMessage(message: Message): Boolean {
        Toast.makeText(this, message.what, Toast.LENGTH_SHORT).show()
        if (message.what != R.string.disconnected) {
            updateForegroundNotification(message.what)
        }
        return true
    }

    private fun connect() {
        updateForegroundNotification(R.string.connecting)
        handler!!.sendEmptyMessage(R.string.connecting)

        val prefs = getSharedPreferences(SettingsPreference.NAME.name, MODE_PRIVATE)
        val server = prefs.getString(SettingsPreference.SERVER_ADDRESS.name, "")
        val secret = prefs.getString(SettingsPreference.SHARED_SECRET.name, "")!!.toByteArray()
        val allow = prefs.getBoolean(SettingsPreference.ALLOW.name, true)
        val packages = prefs.getStringSet(SettingsPreference.PACKAGES.name, mutableSetOf())
        val port = prefs.getInt(SettingsPreference.SERVER_PORT.name, 0)
        val proxyHost = prefs.getString(SettingsPreference.PROXY_HOSTNAME.name, "")
        val proxyPort = prefs.getInt(SettingsPreference.PROXY_PORT.name, 0)
        startConnection(
            SimpleVpnConnection(this, nextConnectionId.getAndIncrement(), server!!, port, secret, proxyHost, proxyPort, allow, packages!!)
        )
    }

    private fun startConnection(connection: SimpleVpnConnection) {
        val thread = Thread(connection, "ToyVpnThread")
        setConnectingThread(thread)

        connection.setConfigureIntent(configureIntent)
        connection.setOnEstablishListener(object : SimpleVpnConnection.OnEstablishListener {
            override fun onEstablish(tunInterface: ParcelFileDescriptor?) {
                handler!!.sendEmptyMessage(R.string.connected)
                connectingThread.compareAndSet(thread, null)
                setConnection(Connection(thread, tunInterface!!))
            }
        })
        thread.start()
    }

    private fun setConnectingThread(thread: Thread?) {
        val oldThread = connectingThread.getAndSet(thread)
        oldThread?.interrupt()
    }

    private fun setConnection(connection: Connection?) {
        val oldConnection = this.connection.getAndSet(connection)
        if (oldConnection != null) {
            try {
                oldConnection.first!!.interrupt()
                oldConnection.second!!.close()
            } catch (e: IOException) {
                Timber.e(e, "Closing VPN interface")
            }
        }
    }

    private fun disconnect() {
        handler!!.sendEmptyMessage(R.string.disconnected)
        setConnectingThread(null)
        setConnection(null)
        stopForeground(true)
    }

    private fun updateForegroundNotification(message: Int) {
        val mNotificationManager = getSystemService(
            NOTIFICATION_SERVICE
        ) as NotificationManager
        mNotificationManager.createNotificationChannel(
            NotificationChannel(
                NOTIFICATION_CHANNEL_ID, NOTIFICATION_CHANNEL_ID,
                NotificationManager.IMPORTANCE_DEFAULT
            )
        )
        startForeground(
            1, Notification.Builder(this, NOTIFICATION_CHANNEL_ID)
                .setSmallIcon(R.drawable.ic_vpn)
                .setContentText(getString(message))
                .setContentIntent(configureIntent)
                .build()
        )
    }
}
