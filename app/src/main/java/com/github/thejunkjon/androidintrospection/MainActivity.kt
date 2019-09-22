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
import com.github.thejunkjon.lib.ApkProcessor
import kotlinx.android.synthetic.main.activity_main.*
import timber.log.Timber.d

class MainActivity : AppCompatActivity() {

    private lateinit var installedApps: List<ResolveInfo>
    private var appIconSize: Int = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        updateInstalledApps()

        installed_apps.adapter = AppsAdapter()
        installed_apps.setOnItemClickListener { adapterView, view, position, l ->
            val clickedItem = installedApps[position]
            d("item clicked : position [$position] item [$clickedItem]")
            ApkProcessor(clickedItem.activityInfo.applicationInfo.sourceDir).makeDebuggable()
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
