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
package com.github.jonforshort.androidintrospection

import android.app.Activity
import android.content.Intent
import android.net.VpnService
import android.os.Bundle
import android.widget.ToggleButton
import com.github.jonforshort.vpn.isVpnRunning
import com.github.jonforshort.vpn.startVpn
import com.github.jonforshort.vpn.stopVpn

class LocalVpnActivity : Activity() {

    private lateinit var enableLocalVpnButton: ToggleButton

    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_local_vpn)

        enableLocalVpnButton = findViewById(R.id.enableVpnToggle)
        enableLocalVpnButton.isChecked = isVpnRunning(this)
        enableLocalVpnButton.setOnClickListener {
            updateVpnState()
        }
    }

    private fun updateVpnState() {
        if (enableLocalVpnButton.isChecked) {
            prepareVpn()
        } else {
            stopVpn(this)
        }
    }

    private fun prepareVpn() {
        val vpnIntent = VpnService.prepare(this)
        if (vpnIntent != null) {
            startActivityForResult(vpnIntent, LOCAL_VPN_REQUEST_CODE)
        } else {
            onActivityResult(LOCAL_VPN_REQUEST_CODE, RESULT_OK, null)
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == LOCAL_VPN_REQUEST_CODE && resultCode == RESULT_OK) {
            startVpn(applicationContext)
        }
    }
}

private const val LOCAL_VPN_REQUEST_CODE = 1000