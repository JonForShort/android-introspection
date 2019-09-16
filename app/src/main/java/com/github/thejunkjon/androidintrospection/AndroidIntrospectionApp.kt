package com.github.thejunkjon.androidintrospection

import android.app.Application
import android.content.Context
import timber.log.Timber
import timber.log.Timber.DebugTree

class AndroidIntrospectionApp : Application() {
    
    override fun attachBaseContext(base: Context) {
        super.attachBaseContext(base)
        Timber.plant(DebugTree())
    }
}