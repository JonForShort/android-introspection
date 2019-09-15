package com.github.thejunkjon.androidintrospection

import android.content.Intent
import android.content.pm.ResolveInfo
import android.os.Bundle
import android.view.View
import android.view.ViewGroup
import android.view.ViewGroup.LayoutParams
import android.view.ViewGroup.LayoutParams.WRAP_CONTENT
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
            val imageView = if (convertView == null) {
                ImageView(this@MainActivity).apply {
                    layoutParams = LayoutParams(WRAP_CONTENT, WRAP_CONTENT);
                    setPadding(10, 10, 10, 10);
                    scaleType = ImageView.ScaleType.CENTER_CROP;
                }
            } else {
                convertView as ImageView
            }
            val resolveInfo = mInstalledApps[position]
            imageView.setImageDrawable(resolveInfo.activityInfo.loadIcon(packageManager))
            return imageView
        }

        override fun getItem(position: Int): Any {
            return mInstalledApps.get(position)
        }

        override fun getItemId(position: Int): Long {
            return position.toLong()
        }
    }
}
