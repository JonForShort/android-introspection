package com.github.thejunkjon.androidintrospection

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.github.thejunkjon.lib.HookManager

class MainActivity : AppCompatActivity() {

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        HookManager().applyHooks()
    }
}
