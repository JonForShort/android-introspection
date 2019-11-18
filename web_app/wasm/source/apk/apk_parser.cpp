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
#include <cstdint>
#include <zip.h>

#include "utils/log.h"

#include "apk_parser.h"

using namespace ai;

namespace {

struct UnzOpenFile {

  explicit UnzOpenFile(const char *szFileName) : zip_(unzOpen(szFileName)) {}

  ~UnzOpenFile() { unzClose(zip_); }

  auto get() const -> unzFile { return zip_; }

private:
  unzFile zip_;
};

struct UnzOpenCurrentFile {

  explicit UnzOpenCurrentFile(unzFile const zipFile) : zipFile_(zipFile), result_(unzOpenCurrentFile(zipFile)) {}

  ~UnzOpenCurrentFile() {
    if (result_ == MZ_OK) {
      unzCloseCurrentFile(zipFile_);
    }
  }

  auto result() const -> int { return result_; }

private:
  unzFile const zipFile_;

  int const result_;
};

} // namespace

auto ApkParser::getFileNames() const -> std::vector<std::string> {
  std::vector<std::string> fileNames;
  auto szPathToApk = pathToApk_.c_str();
  auto const openedZipFile = UnzOpenFile(szPathToApk);
  if (openedZipFile.get() == nullptr) {
    LOGW("openedZipFile.get(), result=nullptr szPathToApk [%s]", szPathToApk);
    return fileNames;
  }
  if (auto result = unzGoToFirstFile(openedZipFile.get()); result != UNZ_OK) {
    LOGW("unzGoToFirstFile, result=[%d] szPathToApk=[%s]", result, szPathToApk);
    return fileNames;
  }
  do {
    unz_file_info zipFileInfo;
    char szFilename[BUFSIZ] = {0};
    if (unzGetCurrentFileInfo(openedZipFile.get(), &zipFileInfo, szFilename, sizeof(szFilename), nullptr, 0, nullptr, 0) == UNZ_OK) {
      auto const fileName = std::string(szFilename);
      fileNames.emplace_back(fileName);
    }
  } while (unzGoToNextFile(openedZipFile.get()) == UNZ_OK);
  return fileNames;
}

auto ApkParser::getFileContents(char const *szZipFileName) const -> std::vector<std::byte> {
  auto contents = std::vector<std::byte>();
  auto const szFileName = pathToApk_.c_str();
  auto const zipFile = UnzOpenFile(szFileName);
  if (zipFile.get() == nullptr) {
    LOGW("zipFile.get(), szFileName=[%s]", szFileName);
    return contents;
  }
  if (auto result = unzLocateFile(zipFile.get(), szZipFileName, nullptr); result != MZ_OK) {
    LOGW("unzLocateFile, result=[%d] szFileName=[%s] szZipFileName=[%s]", result, szFileName, szZipFileName);
    return contents;
  }
  auto const openedZipFile = UnzOpenCurrentFile(zipFile.get());
  if (auto result = openedZipFile.result(); result != MZ_OK) {
    LOGW("unzOpenCurrentFile, result=[%d] szFileName=[%s] szZipFileName=[%s]", result, szFileName, szZipFileName);
    return contents;
  }
  unz_file_info zipFileInfo;
  if (auto result = unzGetCurrentFileInfo(zipFile.get(), &zipFileInfo, nullptr, 0, nullptr, 0, nullptr, 0); result != UNZ_OK) {
    LOGW("unzGetCurrentFileInfo, result=[%d] szFileName=[%s] szZipFileName=[%s]", result, szFileName, szZipFileName);
    return contents;
  }
  auto const compressedContentsSizeInBytes = zipFileInfo.uncompressed_size;
  contents.resize(compressedContentsSizeInBytes);
  unzReadCurrentFile(zipFile.get(), &contents[0], static_cast<uint32_t>(contents.size()));
  return contents;
}

auto ApkParser::setFileContents(char const *fileName, std::vector<std::byte> const &content) const -> void {
  (void)fileName;
  (void)content;
}
