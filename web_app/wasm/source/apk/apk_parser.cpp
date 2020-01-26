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
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <mz.h>

#include "utils/log.h"

#include "apk_parser.h"
#include "scoped_minizip.h"
#include "zip_archiver.h"

using namespace ai;
using namespace ai::minizip;

namespace {

auto writeToTempFile(std::vector<std::byte> const &content) {
  auto tempName = std::vector<char>(L_tmpnam);
  std::tmpnam(&tempName[0]);
  auto tempPath = std::filesystem::temp_directory_path() / tempName.data();
  auto outputFile = std::ofstream(tempPath, std::fstream::binary);
  outputFile.write(reinterpret_cast<char const *>(content.data()), static_cast<uint32_t>(content.size()));
  outputFile.close();
  return tempPath.string();
}

} // namespace

auto ApkParser::getFiles() const -> std::vector<std::string> {
  LOGD("getFileNames");
  return ai::ZipArchiver(pathToApk_).files();
}

auto ApkParser::getFileContents(char const *szFileInArchive) const -> std::vector<std::byte> {
  LOGD("getFileContents, szFileInArchive [%s]", szFileInArchive);
  if (szFileInArchive == nullptr) {
    throw std::invalid_argument("file is null");
  }
  return ai::ZipArchiver(pathToApk_).extract(szFileInArchive);
}

auto ApkParser::setFileContents(char const *szFileInArchive, std::vector<std::byte> const &contents) const -> void {
  LOGD("setFileContents, szFileInArchive [%s] contents [%d]", szFileInArchive, contents.size());
  if (szFileInArchive == nullptr) {
    throw std::invalid_argument("file is null");
  }
  if (contents.empty()) {
    throw std::invalid_argument("contents are empty");
  }
  auto pathToContents = writeToTempFile(contents);
  auto zipArchiver = ai::ZipArchiver(pathToApk_);
  auto inputFile = std::ifstream(pathToContents);
  zipArchiver.add(inputFile, szFileInArchive);
}
