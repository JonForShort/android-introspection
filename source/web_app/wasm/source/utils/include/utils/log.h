//
// MIT License
//
// Copyright 2019-2020
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
#ifndef ANDROID_INTROSPECTION_UTILS_LOG_H_
#define ANDROID_INTROSPECTION_UTILS_LOG_H_

#if LOG_LEVEL == 0
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#elif LOG_LEVEL == 1
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#elif LOG_LEVEL == 2
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#elif LOG_LEVEL == 3
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_WARN
#elif LOG_LEVEL == 4
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_ERROR
#elif LOG_LEVEL == 5
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_CRITICAL
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#endif

#include "spdlog/spdlog.h"

#if LOG_LEVEL < 1
#define LOGV(...) spdlog::trace(__VA_ARGS__)
#else
#define LOGV(...)
#endif

#if LOG_LEVEL < 2
#define LOGD(...) spdlog::debug(__VA_ARGS__)
#else
#define LOGD(...)
#endif

#if LOG_LEVEL < 3
#define LOGI(...) spdlog::info(__VA_ARGS__)
#else
#define LOGI(...)
#endif

#if LOG_LEVEL < 4
#define LOGW(...) spdlog::warn(__VA_ARGS__)
#else
#define LOGW(...)
#endif

#if LOG_LEVEL < 5
#define LOGE(...) spdlog::error(__VA_ARGS__)
#else
#define LOGE(...)
#endif

#if LOG_LEVEL < 6
#define LOGF(...) spdlog::critical(__VA_ARGS__)
#else
#define LOGF(...)
#endif

#endif /* ANDROID_INTROSPECTION_UTILS_LOG_H_ */
