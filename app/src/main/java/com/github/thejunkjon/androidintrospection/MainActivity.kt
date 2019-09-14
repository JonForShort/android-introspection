package com.github.thejunkjon.androidintrospection

import android.content.Intent
import android.content.pm.ResolveInfo
import android.os.Bundle
import android.view.View
import android.view.ViewGroup
import android.widget.BaseAdapter
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*


class MainActivity : AppCompatActivity() {

    private lateinit var mInstalledApps: List<ResolveInfo>

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        updateInstalledApps()

        installed_apps.adapter = AppsAdapter()
    }

    private fun updateInstalledApps() {
        val mainIntent = Intent(Intent.ACTION_MAIN, null)
        mainIntent.addCategory(Intent.CATEGORY_LAUNCHER)

        mInstalledApps = packageManager.queryIntentActivities(mainIntent, 0)
    }

    private inner class AppsAdapter : BaseAdapter() {

        override fun getCount(): Int {
            return mInstalledApps.size
        }

        override fun getView(position: Int, convertView: View?, parent: ViewGroup): View {
            val i: ImageView
            if (convertView == null) {
                i = ImageView(this@MainActivity)
            } else {
                i = convertView as ImageView
            }
            val info = mInstalledApps[position]
            i.setImageDrawable(info.activityInfo.loadIcon(packageManager))
            return i
        }

        override fun getItem(position: Int): Any {
            return mInstalledApps.get(position)
        }

        override fun getItemId(position: Int): Long {
            return position.toLong()
        }
    }
}
