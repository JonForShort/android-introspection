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
#include <mz.h>
#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>
#include <zip.h>

#include "utils/log.h"

#include "apk_parser.h"

using namespace ai;

namespace {

struct ScopedUnzOpenFile {

  explicit ScopedUnzOpenFile(const char *szFileName) : zip_(unzOpen(szFileName)) {}

  ~ScopedUnzOpenFile() { unzClose(zip_); }

  auto get() const -> unzFile { return zip_; }

private:
  unzFile zip_;
};

struct ScopedUnzOpenCurrentFile {

  explicit ScopedUnzOpenCurrentFile(unzFile const zipFile) : zipFile_(zipFile), result_(unzOpenCurrentFile(zipFile)) {}

  ~ScopedUnzOpenCurrentFile() {
    if (result_ == MZ_OK) {
      unzCloseCurrentFile(zipFile_);
    }
  }

  auto result() const -> int { return result_; }

private:
  unzFile const zipFile_;

  int const result_;
};

struct ScopedMzZipWriterDelete {

  explicit ScopedMzZipWriterDelete(void *handle) : handle_(handle) {}

  ~ScopedMzZipWriterDelete() { mz_zip_writer_delete(&handle_); }

private:
  void *handle_;
};

struct ScopedMzZipEntryClose {

  explicit ScopedMzZipEntryClose(void *handle) : handle_(handle) {}

  ~ScopedMzZipEntryClose() { mz_zip_entry_close(&handle_); }

private:
  void *handle_;
};

} // namespace

auto ApkParser::getFileNames() const -> std::vector<std::string> {
  std::vector<std::string> fileNames;
  auto szPathToApk = pathToApk_.c_str();
  auto const openedZipFile = ScopedUnzOpenFile(szPathToApk);
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
  if (szZipFileName == nullptr) {
    LOGE("szZipFileName is null");
    return contents;
  }
  auto const szPathToApk = pathToApk_.c_str();
  auto const zipFile = ScopedUnzOpenFile(szPathToApk);
  if (zipFile.get() == nullptr) {
    LOGW("zipFile.get(), szPathToApk=[%s]", szPathToApk);
    return contents;
  }
  if (auto result = unzLocateFile(zipFile.get(), szZipFileName, nullptr); result != MZ_OK) {
    LOGW("unzLocateFile, result=[%d] szPathToApk=[%s] szZipFileName=[%s]", result, szPathToApk, szZipFileName);
    return contents;
  }
  auto const openedZipFile = ScopedUnzOpenCurrentFile(zipFile.get());
  if (auto result = openedZipFile.result(); result != MZ_OK) {
    LOGW("unzOpenCurrentFile, result=[%d] szPathToApk=[%s] szZipFileName=[%s]", result, szPathToApk, szZipFileName);
    return contents;
  }
  unz_file_info zipFileInfo;
  if (auto result = unzGetCurrentFileInfo(zipFile.get(), &zipFileInfo, nullptr, 0, nullptr, 0, nullptr, 0); result != UNZ_OK) {
    LOGW("unzGetCurrentFileInfo, result=[%d] szPathToApk=[%s] szZipFileName=[%s]", result, szPathToApk, szZipFileName);
    return contents;
  }
  auto const compressedContentsSizeInBytes = zipFileInfo.uncompressed_size;
  contents.resize(compressedContentsSizeInBytes);
  unzReadCurrentFile(zipFile.get(), &contents[0], static_cast<uint32_t>(contents.size()));
  return contents;
}

auto ApkParser::setFileContents(char const *szFileName, std::vector<std::byte> const &content) const -> void {
  if (szFileName == nullptr) {
    LOGE("szFileName is null");
    return;
  }
  if (content.empty()) {
    LOGE("content is empty");
    return;
  }
  auto const szPathToApk = pathToApk_.c_str();
  void *zipWriter = nullptr;
  if (auto result = mz_zip_writer_create(&zipWriter); result == nullptr) {
    LOGW("mz_zip_writer_create, result=nullptr szPathToApk=[%s] szFileName=[%s]", result, szPathToApk, szFileName);
    return;
  }
  ScopedMzZipWriterDelete scopedZipWriterDelete(zipWriter);
  if (auto result = mz_zip_writer_open_file(zipWriter, szPathToApk, 0, 0); result != MZ_OK) {
    LOGW("mz_zip_writer_open_file, result=[%d] szPathToApk=[%s] szFileName=[%s]", result, szPathToApk, szFileName);
    return;
  }
  if (auto result = mz_zip_locate_entry(zipWriter, szFileName, 0); result == MZ_OK) {
    LOGD("file name already exists in zip file; szPathToApk=[%s] szFileName=[%s]", szPathToApk, szFileName);

    mz_zip_file *zipFile;
    if (auto result = mz_zip_entry_get_info(zipWriter, &zipFile); result != MZ_OK) {
      LOGW("mz_zip_entry_get_info, result=[%d] szPathToApk=[%s] szFileName=[%s]", result, szPathToApk, szFileName);
      return;
    }
    if (auto result = mz_zip_writer_entry_open(zipWriter, zipFile); result != MZ_OK) {
      LOGW("mz_zip_writer_entry_open, result=[%d] szPathToApk=[%s] szFileName=[%s]", result, szPathToApk, szFileName);
      return;
    }
    ScopedMzZipEntryClose scopedZipEntryClose(zipWriter);
    if (auto result = mz_zip_entry_write(zipWriter, &content[0], static_cast<int32_t>(content.size())); result != MZ_OK) {
      LOGW("mz_zip_entry_write, result=[%d] szPathToApk=[%s] szFileName=[%s]", result, szPathToApk, szFileName);
      return;
    }
  } else {
    LOGD("file name does not exist in zip file; szPathToApk=[%s] szFileName=[%s]", szPathToApk, szFileName);

    mz_zip_file fileInfo = {};
    fileInfo.creation_date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    fileInfo.modified_date = fileInfo.creation_date;
    fileInfo.accessed_date = fileInfo.creation_date;
    fileInfo.uncompressed_size = static_cast<long>(content.size());
    fileInfo.filename = szFileName;
    fileInfo.filename_size = static_cast<uint16_t>(strlen(szFileName));
    fileInfo.flag = MZ_ZIP_FLAG_UTF8;
    // PKWARE .ZIP File Format Specification version 6.3.x
    const uint16_t ZIP_SPECIFICATION_VERSION_CODE = 63;
    fileInfo.version_madeby = MZ_HOST_SYSTEM_UNIX << 8 | ZIP_SPECIFICATION_VERSION_CODE;
    // Compression options.
    fileInfo.compression_method = MZ_COMPRESS_METHOD_DEFLATE;
    fileInfo.zip64 = MZ_ZIP64_AUTO;

    if (auto result = mz_zip_entry_write_open(zipWriter, &fileInfo, MZ_COMPRESS_LEVEL_DEFAULT, 0, nullptr); result != MZ_OK) {
      LOGW("mz_zip_entry_write_open, result=[%d] szPathToApk=[%s] szFileName=[%s]", result, szPathToApk, szFileName);
      return;
    }
    if (auto result = mz_zip_entry_write(zipWriter, &content[0], content.size()); result != MZ_OK) {
      LOGW("mz_zip_entry_write, result=[%d] szPathToApk=[%s] szFileName=[%s]", result, szPathToApk, szFileName);
      return;
    }
  }
}
