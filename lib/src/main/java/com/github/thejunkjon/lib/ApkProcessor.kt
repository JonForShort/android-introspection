package com.github.thejunkjon.lib

import java.lang.System.loadLibrary

class ApkProcessor(private val apkPath: String) {

    init {
        loadLibrary("native-lib")
    }

    fun makeDebuggable(): Boolean {
        return makeDebuggableNative(apkPath)
    }

    private external fun makeDebuggableNative(apkPath: String): Boolean
}