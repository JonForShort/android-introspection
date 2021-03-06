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
#ifndef ANDROID_INTROSPECTION_APK_ZIP_ARCHIVER_H_
#define ANDROID_INTROSPECTION_APK_ZIP_ARCHIVER_H_

#include <string>
#include <string_view>
#include <vector>

namespace ai {

class ZipArchiver final {
  std::string const zipPath_;

public:
  explicit ZipArchiver(std::string_view zipPath) : zipPath_(zipPath) {}

  auto add(std::istream &source, std::string_view pathInArchive) const -> void;

  auto files() const -> std::vector<std::string>;

  auto contains(std::string_view pathInArchive) const -> bool;

  auto extractAll(std::string_view destinationDirectory) const -> void;

  auto extract(std::string_view pathInArchive, std::string_view destinationDirectory) const -> void;

  auto extract(std::string_view pathInArchive) const -> std::vector<std::byte>;
};

} // namespace ai

#endif /* ANDROID_INTROSPECTION_APK_ZIP_ARCHIVER_H_ */
