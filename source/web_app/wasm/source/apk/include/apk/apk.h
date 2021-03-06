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
#ifndef ANDROID_INTROSPECTION_APK_APK_H_
#define ANDROID_INTROSPECTION_APK_APK_H_

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "apk_exception.h"

namespace ai {

class Apk final {
public:
  explicit Apk(std::string_view apkPath);

  ~Apk();

  auto isValid() const -> bool;

  auto makeDebuggable() const -> void;

  auto isDebuggable() const -> bool;

  auto getAndroidManifest() const -> std::string;

  auto getFiles() const -> std::vector<std::string>;

  auto getFileContent(std::string_view filePath) const -> std::vector<std::byte>;

  auto getProperties() const -> std::map<std::string, std::string>;

  auto dump(std::string_view destinationDirectory) const -> void;

private:
  class ApkImpl;

  std::unique_ptr<ApkImpl> const pimpl_;
};

} // namespace ai

#endif /* ANDROID_INTROSPECTION_APK_APK_H_ */
