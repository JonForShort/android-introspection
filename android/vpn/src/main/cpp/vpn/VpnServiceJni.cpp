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
#include <jni.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/android_sink.h>

#include "VpnService.h"

namespace {
    auto const gLogger = spdlog::android_logger_mt("android", "VpnServiceJni");
}

extern "C" JNIEXPORT void JNICALL
Java_com_github_thejunkjon_vpn_NativeVpnService_start(JNIEnv *env, jobject thiz, jint fd) {
    gLogger->info("NativeVpnService::star");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_github_thejunkjon_vpn_NativeVpnService_stop(JNIEnv *env, jobject thiz) {
    gLogger->info("NativeVpnService::star");
}
