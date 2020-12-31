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

#include "apk_analyzer/apk_analyzer.h"

using namespace ai;

auto ApkAnalyzer::isApkDebuggable(char const *pathToApk) const -> bool {
  auto command = std::string(pathToApkAnalyzer_) + " manifest debuggable " + pathToApk;
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

auto ApkAnalyzer::getAndroidManifest(char const *pathToApk) const -> std::string {
  auto command = std::string(pathToApkAnalyzer_) + " manifest print " + pathToApk;
  auto handle = popen(command.c_str(), "r");
  if (handle == nullptr) {
    return "";
  }

  std::string bufferString;
  static constexpr auto bufferSize = 80;
  char bufferBytes[bufferSize] = {0};
  while (fgets(bufferBytes, bufferSize, handle)) {
    bufferString += bufferBytes;
  }

  auto exitStatus = WEXITSTATUS(pclose(handle));
  return exitStatus == 0 ? bufferString : "";
}