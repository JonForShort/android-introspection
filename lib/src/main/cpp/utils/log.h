#ifndef ANDROID_INTROSPECTION_LOG_H_
#define ANDROID_INTROSPECTION_LOG_H_

#include <android/log.h>

namespace {
    auto constexpr tag = "AndroidIntrospectionNative";
}

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, tag,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , tag,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , tag,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , tag,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , tag,__VA_ARGS__)

#endif /* ANDROID_INTROSPECTION_LOG_H_ */
