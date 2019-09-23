#include <jni.h>
#include <string>

#include "log.h"
#include "utils.h"

#define BEGIN_EXTERN_C extern "C" {
#define END_EXTERN_C }

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

    auto const *apkPath = env->GetStringUTFChars(apkPathJni, 0);
    if (ai::utils::fileExists(apkPath)) {
        LOGD("apk path exists");
    } else {
        LOGD("apk path does not exist");
    }

    env->ReleaseStringUTFChars(apkPathJni, apkPath);

    return JNI_TRUE;
}

END_EXTERN_C