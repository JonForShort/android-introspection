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
#include <string>

#include "apk/apk.h"
#include "apk_parser.h"
#include "binary_xml.h"
#include "utils/log.h"

using namespace ai;

namespace {

static constexpr char const *const ANDROID_MANIFEST = "AndroidManifest.xml";

static constexpr char const *const ANDROID_MANIFEST_TAG_APPLICATION = "application";

} // namespace

auto Apk::makeDebuggable() const -> void {
  auto const apkParser = ai::ApkParser(apkPath_);
  auto const fileNames = apkParser.getFileNames();
  auto const missingAndroidManifest = std::find(fileNames.cbegin(), fileNames.cend(), ANDROID_MANIFEST) == fileNames.end();
  if (!missingAndroidManifest) {
    LOGD("unable to find manifest in [%s]", apkPath_);
    throw MissingAndroidManifestException(apkPath_);
  }

  auto const contents = apkParser.getFileContents(ANDROID_MANIFEST);
  if (contents.empty()) {
    LOGW("unable to read [%s]", apkPath_);
    throw MissingAndroidManifestException(apkPath_);
  }

  auto const binaryXml = BinaryXml(contents);
  auto const strings = binaryXml.readStrings();
  if (strings.empty()) {
    LOGW("unable to parse strings in [%s]", apkPath_);
    throw MalformedAndroidManifestException(apkPath_);
  }

  auto const hasApplicationTag = std::find(strings.cbegin(), strings.cend(), ANDROID_MANIFEST_TAG_APPLICATION) != strings.end();
  if (!hasApplicationTag) {
    LOGW("unable to find application tag in [%s]", apkPath_);
    throw MalformedAndroidManifestException(apkPath_);
  }

  struct MyBinaryXmlVisitor final : public BinaryXmlVisitor {
    char const *const apkPath_;

    MyBinaryXmlVisitor(char const *const apkPath) noexcept : apkPath_(apkPath) {}

    auto visit(StartXmlTagElement const &element) const -> void override {
      auto const tag = element.tag();
      LOGD("traverse start tag element [%s]", tag);
      if (tag == ANDROID_MANIFEST_TAG_APPLICATION) {
        LOGD("found application tag");
      }
    }

    auto visit(EndXmlTagElement const &element) const -> void override {
      auto const tag = element.tag();
      LOGD("traverse end tag element [%s]", tag);
      if (tag == ANDROID_MANIFEST_TAG_APPLICATION) {
        LOGD("found application tag");
      }
    }

    auto visit(InvalidXmlTagElement const &element) const -> void override {
      LOGW("traverse invalid element [%s]", element.error());
      throw MalformedAndroidManifestException(apkPath_);
    }

  } visitor(apkPath_);

  binaryXml.traverseXml(visitor);
}

auto Apk::isDebuggable() const -> bool { return false; }
