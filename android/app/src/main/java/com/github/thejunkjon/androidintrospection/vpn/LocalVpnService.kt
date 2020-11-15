//
// MIT License
//
// Copyright 2020
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
package com.github.thejunkjon.androidintrospection.vpn

import android.app.ActivityManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.net.VpnService
import android.os.ParcelFileDescriptor
import com.github.thejunkjon.androidintrospection.R
import timber.log.Timber.d
import timber.log.Timber.e
import java.io.IOException
import java.net.NetworkInterface

fun startVpn(context: Context) {
    val intent = Intent(context, LocalVpnService::class.java).apply {
        action = "START_VPN"
    }
    context.startService(intent)
}

fun stopVpn(context: Context) {
    val intent = Intent(context, LocalVpnService::class.java).apply {
        action = "STOP_VPN"
    }
    context.startService(intent)
}

fun isVpnRunning(context: Context) = isVpnTunnelUp() && isVpnServiceRunning(context)

@Suppress("DEPRECATION")
private fun isVpnServiceRunning(context: Context) =
    (context.getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager)
        .getRunningServices(Integer.MAX_VALUE)
        .any { it.service.className == LocalVpnService::class.java.name }

private fun isVpnTunnelUp(): Boolean {
    val networkInterfaces = NetworkInterface.getNetworkInterfaces()
    for (networkInterface in networkInterfaces) {
        if (networkInterface.displayName == "tun0" && networkInterface.isUp) {
            return true
        }
    }
    return false
}

class LocalVpnService : VpnService() {

    private lateinit var vpnInterface: ParcelFileDescriptor
    private lateinit var configureIntent: PendingIntent

    companion object {
        private const val VPN_ADDRESS = "10.0.0.2"
        private const val VPN_ROUTE = "0.0.0.0"
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        if (intent?.action == "STOP_VPN") {
            stopVpn()
        }
        return START_STICKY
    }

    private fun stopVpn() {
        try {
            vpnInterface.close()
        } catch (e: IOException) {
            e(e, "unable to close parcel file descriptor")
        }
        stopForeground(true)
        stopSelf()
    }

    override fun onCreate() {
        super.onCreate()
        d("onCreate called")

        configureIntent = createConfigureIntent(this)

        createVpnInterface()
    }

    private fun createVpnInterface() {
        d("creating vpn interface")

        val vpnServiceBuilder = super.Builder()
        vpnServiceBuilder.addAddress(VPN_ADDRESS, 32)
        vpnServiceBuilder.addRoute(VPN_ROUTE, 0)

        val vpnName = getString(R.string.app_name)
        vpnInterface = vpnServiceBuilder
            .setSession(vpnName)
            .setConfigureIntent(configureIntent)
            .establish()!!
    }

    override fun onDestroy() {
        super.onDestroy()
        d("onDestroy called")
    }
}