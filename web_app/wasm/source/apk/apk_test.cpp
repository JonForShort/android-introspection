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
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "apk/apk.h"
#include "apk_analyzer/apk_analyzer.h"
#include "utils/log.h"
#include "zip_archiver.h"

#include "apk_parser.h"

namespace fs = std::filesystem;

namespace {

struct TestEnvironment {
  std::string androidHomeDir;
  std::string testsDir;
};

std::unique_ptr<TestEnvironment> gTestEnvironment;

auto setEnvironmentIfReady() -> bool {
  auto const androidHome = std::getenv("AI_ANDROID_HOME");
  if (androidHome == nullptr) {
    LOGE("android home is not defined");
    return false;
  }
  auto const testsDir = std::getenv("AI_TESTS_DIR");
  if (testsDir == nullptr) {
    LOGE("tests directory is not defined");
    return false;
  }

  gTestEnvironment = std::make_unique<TestEnvironment>();
  gTestEnvironment->androidHomeDir = androidHome;
  gTestEnvironment->testsDir = testsDir;

  return true;
}

struct ScopedFileDeleter {
  ScopedFileDeleter(char const *path) : path_(path) {}

  ~ScopedFileDeleter() { fs::remove(path_); }

private:
  char const *path_;
};

fs::path getTestApkPath(char const *fileName) { return fs::path(gTestEnvironment->testsDir) / "resources" / "apks" / fileName; }

} // namespace

TEST(MakeDebuggable, MakeReleaseApkIsDebuggable_ApkIsMadeDebuggableSuccessfully) {
  auto originalTestApk = getTestApkPath("test_release.apk");
  auto copiedTestApk = fs::temp_directory_path() / originalTestApk.filename();
  auto isCopiedSuccessfully = fs::copy_file(originalTestApk, copiedTestApk, fs::copy_options::overwrite_existing);
  EXPECT_TRUE(isCopiedSuccessfully);
  {
    auto scopedFileDeleter = ScopedFileDeleter(copiedTestApk.c_str());

    auto apk = ai::Apk(copiedTestApk.c_str());
    EXPECT_FALSE(apk.isDebuggable());

    EXPECT_NO_THROW(apk.makeDebuggable());
    EXPECT_TRUE(apk.isDebuggable());

    auto pathToApkAnalyzer = fs::path(gTestEnvironment->androidHomeDir) / "tools" / "bin" / "apkanalyzer";
    auto apkAnalyzer = ai::ApkAnalyzer(pathToApkAnalyzer.string().c_str());
    auto isDebuggable = apkAnalyzer.isApkDebuggable(copiedTestApk.c_str());
    EXPECT_TRUE(isDebuggable);
  }
}

TEST(ApkParser, ReleaseApkContainsAndroidManifest_AndroidManifestFoundSuccessfully) {
  auto pathToApk = getTestApkPath("test_release.apk");
  auto apkParser = ai::ApkParser(pathToApk.string().c_str());

  auto fileNames = apkParser.getFileNames();
  EXPECT_TRUE(!fileNames.empty());

  auto maybeAndroidManifest = std::find(fileNames.begin(), fileNames.end(), "AndroidManifest.xml");
  EXPECT_TRUE(maybeAndroidManifest != fileNames.end());
}

TEST(ApkParser, ReleaseApkContainsNonExistantFile_FileNotFoundError) {
  auto pathToApk = getTestApkPath("test_release.apk");
  auto apkParser = ai::ApkParser(pathToApk.string().c_str());

  auto fileNames = apkParser.getFileNames();
  EXPECT_TRUE(!fileNames.empty());

  auto maybeNonExistingFile = std::find(fileNames.begin(), fileNames.end(), "NonExistingFile.xml");
  EXPECT_TRUE(maybeNonExistingFile == fileNames.end());
}

TEST(ApkParser, AddFileToApk_FileIsAddedSuccessfully) {
  auto pathToOriginalApk = getTestApkPath("test_release.apk");
  auto pathToCopiedApk = fs::temp_directory_path() / pathToOriginalApk.filename();
  auto isCopiedSuccessfully = fs::copy_file(pathToOriginalApk, pathToCopiedApk, fs::copy_options::overwrite_existing);
  EXPECT_TRUE(isCopiedSuccessfully);

  auto apkParser = ai::ApkParser(pathToCopiedApk.string().c_str());
  auto contents = std::vector<std::byte>();
  contents.push_back(std::byte(0x1));
  apkParser.setFileContents("test_file", contents);

  auto fileNames = apkParser.getFileNames();
  auto maybeTestFile = std::find(fileNames.begin(), fileNames.end(), "test_file");
  EXPECT_TRUE(maybeTestFile != fileNames.end());
}

TEST(ZipArchiver, AddFileToZip_FileIsAddedSuccessfully) {
  auto testZipFilePath = fs::temp_directory_path() / "AddFileToZip_FileIsAddedSuccessfully";
  auto testZipFileStream = std::ofstream(testZipFilePath.string());
  testZipFileStream << "test";
  testZipFileStream.close();

  auto zipArchiver = ai::ZipArchiver(testZipFilePath);

  zipArchiver.createArchive();
  EXPECT_TRUE(fs::exists(testZipFilePath));

  zipArchiver.deleteArchive();
  EXPECT_FALSE(fs::exists(testZipFilePath));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  if (setEnvironmentIfReady()) {
    return RUN_ALL_TESTS();
  } else {
    return -1;
  }
}
