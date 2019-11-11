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
#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <string>

#include "apk/apk.h"

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
    return false;
  }
  auto const testsDir = std::getenv("AI_TESTS_DIR");
  if (testsDir == nullptr) {
    return false;
  }

  gTestEnvironment = std::make_unique<TestEnvironment>();
  gTestEnvironment->androidHomeDir = androidHome;
  gTestEnvironment->testsDir = testsDir;

  return true;
}

auto checkIfDebuggable(char const *apkPath) -> bool {
  auto apkAnalyzer = fs::path(gTestEnvironment->androidHomeDir) / "tools" / "bin" / "apkanalyzer";
  auto command = apkAnalyzer.string() + " manifest debuggable " + apkPath;
  auto handle = popen(command.c_str(), "r");
  if (handle == nullptr) {
    return false;
  }

  std::string bufferString;
  static constexpr auto bufferSize = 80;
  char bufferBytes[bufferSize] = {0};
  while (fgets(bufferBytes, bufferSize, handle)) {
    bufferString += bufferBytes;
  }

  auto exitStatus = WEXITSTATUS(pclose(handle));
  return exitStatus == 0 && bufferString == "true";
}

} // namespace

TEST(MakeDebuggable, ReleaseApkIsDebuggable) {
  auto testApk = fs::path(gTestEnvironment->testsDir) / "apks" / "test_release.apk";
  auto isSuccessful = ai::apk::makeApkDebuggable(testApk.c_str());
  EXPECT_TRUE(isSuccessful);

  auto isDebuggable = checkIfDebuggable(testApk.c_str());
  EXPECT_TRUE(isDebuggable);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  if (setEnvironmentIfReady()) {
    return RUN_ALL_TESTS();
  } else {
    return -1;
  }
}
