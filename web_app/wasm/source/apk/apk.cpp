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
#include <filesystem>
#include <map>
#include <string>

#include "android_manifest_parser.h"
#include "apk/apk.h"
#include "apk_parser.h"
#include "binary_xml/binary_xml.h"
#include "utils/log.h"
#include "utils/macros.h"
#include "utils/utils.h"

using namespace ai;

namespace fs = std::filesystem;

namespace {

static constexpr char const *const ANDROID_MANIFEST = "AndroidManifest.xml";

} // namespace

class Apk::ApkImpl final {
public:
  ApkImpl(std::string_view apkPath) : apkPath_(apkPath) {}

  auto isValid() const -> bool {
    auto const apkParser = ai::ApkParser(apkPath_);
    auto const files = apkParser.getFiles();
    auto const hasAndroidManifest = std::find(files.cbegin(), files.cend(), ANDROID_MANIFEST) != files.end();
    if (!hasAndroidManifest) {
      LOGW("unable to find manifest in [{}]", apkPath_);
      return false;
    }
    auto const contents = apkParser.getFileContents(ANDROID_MANIFEST);
    if (contents.empty()) {
      LOGW("unable to read [{}]", apkPath_);
      return false;
    }
    return AndroidManifestParser(contents).isValid();
  }

  auto makeDebuggable() const -> void {
    auto const androidManifestContents = getFileContent(ANDROID_MANIFEST);
    auto const androidManifestParser = AndroidManifestParser(androidManifestContents);
    return androidManifestParser.setApplicationDebuggable(true);
  }

  auto isDebuggable() const -> bool {
    auto const androidManifestContents = getFileContent(ANDROID_MANIFEST);
    auto const androidManifestParser = AndroidManifestParser(androidManifestContents);
    return androidManifestParser.isApplicationDebuggable();
  }

  auto getAndroidManifest() const -> std::string {
    auto const androidManifestContents = getFileContent(ANDROID_MANIFEST);
    auto const androidManifestParser = AndroidManifestParser(androidManifestContents);
    return androidManifestParser.toStringXml();
  }

  auto getFiles() const -> std::vector<std::string> {
    auto const apkParser = ai::ApkParser(apkPath_);
    return apkParser.getFiles();
  }

  auto getFileContent(std::string_view filePath) const -> std::vector<std::byte> {
    auto const apkParser = ai::ApkParser(apkPath_);
    return apkParser.getFileContents(filePath);
  }

  auto getProperties() const -> std::map<std::string, std::string> {
    auto const isApkValid = isValid();
    auto properties = std::map<std::string, std::string>{{"valid", isApkValid ? "true" : "false"}};

    if (isValid()) {
      auto const androidManifestContents = getFileContent(ANDROID_MANIFEST);
      auto const androidManifestParser = AndroidManifestParser(androidManifestContents);

      properties.insert({"debuggable", androidManifestParser.isApplicationDebuggable() ? "true" : "false"});
      properties.insert({"manifest", androidManifestParser.toStringXml()});
      properties.insert({"packageName", androidManifestParser.getPackageName()});
      properties.insert({"versionCode", androidManifestParser.getVersionCode()});
      properties.insert({"versionName", androidManifestParser.getVersionName()});
    }

    return properties;
  }

  auto dump(std::string_view destinationDirectory) const -> void {
    fs::create_directories(destinationDirectory);
    auto const androidManifest = getAndroidManifest();
    fs::path androidManifestFile = fs::path(destinationDirectory) / ANDROID_MANIFEST;
    utils::writeToFile(androidManifestFile, androidManifest);
  }

private:
  std::string const apkPath_;
};

Apk::Apk(std::string_view apkPath) : pimpl_(std::make_unique<Apk::ApkImpl>(apkPath)) {}

Apk::~Apk() = default;

auto Apk::isValid() const -> bool { return pimpl_->isValid(); }

auto Apk::makeDebuggable() const -> void { return pimpl_->makeDebuggable(); }

auto Apk::isDebuggable() const -> bool { return pimpl_->isDebuggable(); }

auto Apk::getAndroidManifest() const -> std::string { return pimpl_->getAndroidManifest(); }

auto Apk::getFiles() const -> std::vector<std::string> { return pimpl_->getFiles(); }

auto Apk::getFileContent(std::string_view filePath) const -> std::vector<std::byte> { return pimpl_->getFileContent(filePath); }

auto Apk::getProperties() const -> std::map<std::string, std::string> { return pimpl_->getProperties(); }

auto Apk::dump(std::string_view destinationDirectory) const -> void { return pimpl_->dump(destinationDirectory); }
