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
#include <jni.h>
#include <string>

#include "apk/apk.h"
#include "utils/log.h"
#include "utils/utils.h"

#define BEGIN_EXTERN_C extern "C" {
#define END_EXTERN_C }

using namespace ai;

BEGIN_EXTERN_C

JNIEXPORT jstring JNICALL
Java_com_github_thejunkjon_lib_HookManager_applyHooks(
        JNIEnv *env, jobject) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jboolean JNICALL
Java_com_github_thejunkjon_lib_ApkProcessor_makeDebuggableNative(
        JNIEnv *env, jobject, jstring apkPathJni) {

    LOGD("makeDebuggable invoked");

    auto result = static_cast<jboolean>(JNI_TRUE);
    auto const *apkPath = env->GetStringUTFChars(apkPathJni, 0);
    if (utils::fileExists(apkPath)) {
        LOGD("apk path exists");
        apk::make_apk_debuggable(apkPath);
    } else {
        LOGD("apk path does not exist");
        result = JNI_FALSE;
    }

    env->ReleaseStringUTFChars(apkPathJni, apkPath);

    return result;
}

END_EXTERN_C
