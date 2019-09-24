#ifndef ANDROID_INTROSPECTION_LOG_H_
#define ANDROID_INTROSPECTION_LOG_H_

#include <android/log.h>

#define TAG "AndroidIntrospectionNative"

#if LOG_LEVEL < 1
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#else
#define LOGV(...)
#endif

#if LOG_LEVEL < 2
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#else
#define LOGD(...)
#endif

#if LOG_LEVEL < 3
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#else
#define LOGI(...)
#endif

#if LOG_LEVEL < 4
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#else
#define LOGW(...)
#endif

#if LOG_LEVEL < 5
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#else
#define LOGE(...)
#endif

#if LOG_LEVEL < 6
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, TAG, __VA_ARGS__)
#else
#define LOGF(...)
#endif

#endif /* ANDROID_INTROSPECTION_LOG_H_ */
