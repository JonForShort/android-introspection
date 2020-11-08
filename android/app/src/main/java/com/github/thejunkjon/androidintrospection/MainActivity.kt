//
// MIT License
//
// Copyright 2019
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
package com.github.thejunkjon.androidintrospection

import android.content.Intent
import android.content.pm.ResolveInfo
import android.os.Bundle
import android.view.View
import android.view.ViewGroup
import android.view.ViewGroup.LayoutParams
import android.widget.BaseAdapter
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import com.github.thejunkjon.androidintrospection.vpn.LocalVpnActivity
import com.github.thejunkjon.lib.ApkProcessor
import kotlinx.android.synthetic.main.activity_main.*
import timber.log.Timber.d
import java.io.File

class MainActivity : AppCompatActivity() {

    private lateinit var installedApps: List<ResolveInfo>
    private var appIconSize: Int = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        updateInstalledApps()

        findViewById<View>(R.id.launchVpnButton).setOnClickListener {
            startActivity(Intent(this, LocalVpnActivity::class.java))
        }

        installed_apps.adapter = AppsAdapter()
        installed_apps.setOnItemClickListener { _, _, position, _ ->
            val clickedItem = installedApps[position]
            d("item clicked : position [$position] item [$clickedItem]")
            val modifiedApkFile = File.createTempFile("makeDebuggable", ".apk")
            val apkFile = File(clickedItem.activityInfo.applicationInfo.sourceDir)
            ApkProcessor(apkFile).process(modifiedApkFile = modifiedApkFile, makeDebuggable = true)
        }
    }

    private fun updateInstalledApps() {
        val mainIntent = Intent(Intent.ACTION_MAIN, null)
        mainIntent.addCategory(Intent.CATEGORY_LAUNCHER)

        installedApps = packageManager.queryIntentActivities(mainIntent, 0)

        appIconSize = 0
        installedApps.forEach {
            val icon = it.activityInfo.loadIcon(packageManager)
            if (icon.intrinsicHeight >= appIconSize) {
                appIconSize = icon.intrinsicHeight
            }
            if (icon.intrinsicWidth >= appIconSize) {
                appIconSize = icon.intrinsicWidth
            }
        }
    }

    private inner class AppsAdapter : BaseAdapter() {

        override fun getCount() = installedApps.size

        override fun getItem(position: Int) = installedApps[position]

        override fun getItemId(position: Int) = position.toLong()

        override fun getView(position: Int, convertView: View?, parent: ViewGroup): View {
            val imageView = if (convertView == null)
                ImageView(this@MainActivity).apply {
                    layoutParams = LayoutParams(appIconSize, appIconSize)
                } else convertView as ImageView
            val resolveInfo = installedApps[position]
            val icon = resolveInfo.activityInfo.loadIcon(packageManager)
            imageView.setImageDrawable(icon)
            return imageView
        }
    }
}
