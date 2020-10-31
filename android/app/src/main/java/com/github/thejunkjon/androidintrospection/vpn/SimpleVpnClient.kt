package com.github.thejunkjon.androidintrospection.vpn

import android.app.Activity
import android.content.Intent
import android.content.SharedPreferences
import android.content.pm.PackageInfo
import android.net.VpnService
import android.os.Bundle
import android.view.View
import android.widget.RadioButton
import android.widget.TextView
import android.widget.Toast
import com.github.thejunkjon.androidintrospection.R
import java.util.stream.Collectors

class SimpleVpnClient : Activity() {

    internal enum class SettingsPreference {
        NAME, SERVER_ADDRESS, SERVER_PORT, SHARED_SECRET, PROXY_HOSTNAME, PROXY_PORT, ALLOW, PACKAGES;
    }

    private val settingsPreferences: SharedPreferences = getSharedPreferences(SettingsPreference.NAME.name, MODE_PRIVATE)

    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_vpn_client)

        val serverAddress = findViewById<TextView>(R.id.address)
        serverAddress.text = settingsPreferences.getString(SettingsPreference.SERVER_ADDRESS.name, "")

        val serverPort = findViewById<TextView>(R.id.port)
        serverPort.text = settingsPreferences.getInt(SettingsPreference.SERVER_PORT.name, 0).toString()

        val sharedSecret = findViewById<TextView>(R.id.secret)
        sharedSecret.text = settingsPreferences.getString(SettingsPreference.SHARED_SECRET.name, "")

        val proxyHost = findViewById<TextView>(R.id.proxyhost)
        proxyHost.text = settingsPreferences.getString(SettingsPreference.PROXY_HOSTNAME.name, "")

        val proxyPort = findViewById<TextView>(R.id.proxyport)
        proxyPort.text = settingsPreferences.getInt(SettingsPreference.PROXY_PORT.name, 0).toString()

        val allowed = findViewById<RadioButton>(R.id.allowed)
        allowed.isChecked = settingsPreferences.getBoolean(SettingsPreference.ALLOW.name, true)

        val packages = findViewById<TextView>(R.id.packages)
        packages.text = settingsPreferences.getStringSet(SettingsPreference.PACKAGES.name, mutableSetOf<String>())?.joinToString { ", " }

        findViewById<View>(R.id.connect).setOnClickListener {
            if (!checkProxyConfigs(proxyHost.text.toString(), proxyPort.text.toString())) {
                return@setOnClickListener
            }
            val packageSet = packages.text.toString().split(",".toRegex())
                .map { it.trim() }
                .filter { it.isNotEmpty() }
                .toSet()
            if (!checkPackages(packageSet)) {
                return@setOnClickListener
            }
            val serverPortNum = try {
                serverPort.text.toString().toInt()
            } catch (e: NumberFormatException) {
                0
            }
            val proxyPortNum = try {
                proxyPort.text.toString().toInt()
            } catch (e: NumberFormatException) {
                0
            }
            settingsPreferences.edit()
                .putString(SettingsPreference.SERVER_ADDRESS.name, serverAddress.text.toString())
                .putInt(SettingsPreference.SERVER_PORT.name, serverPortNum)
                .putString(SettingsPreference.SHARED_SECRET.name, sharedSecret.text.toString())
                .putString(SettingsPreference.PROXY_HOSTNAME.name, proxyHost.text.toString())
                .putInt(SettingsPreference.PROXY_PORT.name, proxyPortNum)
                .putBoolean(SettingsPreference.ALLOW.name, allowed.isChecked)
                .putStringSet(SettingsPreference.PACKAGES.name, packageSet)
                .apply()
            val intent = VpnService.prepare(this@SimpleVpnClient)
            if (intent != null) {
                startActivityForResult(intent, 0)
            }
        }
        findViewById<View>(R.id.disconnect).setOnClickListener {
            startService(serviceIntent.setAction(SimpleVpnService.ACTION_DISCONNECT))
        }
    }

    private fun checkProxyConfigs(proxyHost: String, proxyPort: String): Boolean {
        val hasIncompleteProxyConfigs = proxyHost.isEmpty() != proxyPort.isEmpty()
        if (hasIncompleteProxyConfigs) {
            Toast.makeText(this, R.string.incomplete_proxy_settings, Toast.LENGTH_SHORT).show()
        }
        return !hasIncompleteProxyConfigs
    }

    private fun checkPackages(packageNames: Set<String>): Boolean {
        val hasCorrectPackageNames = packageNames.isEmpty() ||
                packageManager.getInstalledPackages(0).stream()
                    .map<Any> { pi: PackageInfo -> pi.packageName }
                    .collect(Collectors.toSet())
                    .containsAll(packageNames)
        if (!hasCorrectPackageNames) {
            Toast.makeText(this, R.string.unknown_package_names, Toast.LENGTH_SHORT).show()
        }
        return hasCorrectPackageNames
    }

    override fun onActivityResult(request: Int, result: Int, data: Intent) {
        if (result == RESULT_OK) {
            startService(serviceIntent.setAction(SimpleVpnService.ACTION_CONNECT))
        }
    }

    private val serviceIntent = Intent(this, SimpleVpnService::class.java)
}