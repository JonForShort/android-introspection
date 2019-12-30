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

#include "mz.h"
#include "mz_strm.h"
#include "mz_zip.h"
#include "mz_zip_rw.h"
#include "scoped_minizip.h"
#include "utils/log.h"
#include "utils/utils.h"
#include "zip_archiver.h"

using namespace ai;

using ai::minizip::ScopedMzStreamClose;

namespace {

class ZipException : public std::logic_error {
  int64_t const errorCode_;

public:
  explicit ZipException(char const *const errorMessage, int64_t errorCode) : std::logic_error(errorMessage), errorCode_(errorCode) {}

  auto errorCode() -> int64_t { return errorCode_; }
};

auto getZipHandle(std::string const &archivePath) -> void * {
  void *handle = nullptr;
  if (auto result = mz_stream_open(handle, archivePath.c_str(), MZ_OPEN_MODE_READWRITE | MZ_OPEN_MODE_CREATE); result != MZ_OK) {
    LOGW("mz_stream_open, path [%s]", archivePath.c_str());
    throw ZipException("mz_stream_open", result);
  }
  return handle;
}

} // namespace

auto ZipArchiver::createArchive() const -> void {
  LOGD("createArchive, path [%s]", zipPath_);
  auto *handle = getZipHandle(zipPath_);
  auto scopedStreamClose = ScopedMzStreamClose(handle);
}

auto ZipArchiver::deleteArchive() const -> void {
  LOGD("deleteArchive, path [%s]", zipPath_);
  std::filesystem::remove(zipPath_);
}

auto ZipArchiver::createFile(std::string_view const fileName, std::string_view const filePath) const -> void {
  LOGD("createFile, creating file [%s]", filePath);
  auto *handle = getZipHandle(zipPath_);
  auto scopedStreamClose = ScopedMzStreamClose(handle);
  auto fileNameAsString = std::string(fileName);
  auto filePathAsString = std::string(filePath);
  if (auto result = mz_zip_writer_add_file(handle, filePathAsString.c_str(), fileNameAsString.c_str()); result != MZ_OK) {
    LOGW("mz_zip_writer_add_file, result [%d] zipPath [%s] fileName [%s] filePath [%s]", result, zipPath_.c_str(), fileName, filePath);
    throw ZipException("mz_stream_open", result);
  }
}

auto ZipArchiver::createDirectory(std::string_view const directoryPath) const -> void {
  LOGD("createDirectory, path [%s]", directoryPath);
  auto *handle = getZipHandle(zipPath_);
  auto scopedStreamClose = ScopedMzStreamClose(handle);
  auto directoryPathAsString = std::string(directoryPath);
  if (auto result = mz_zip_writer_add_path(handle, directoryPathAsString.c_str(), nullptr, 1, 1); result != MZ_OK) {
    LOGW("mz_zip_writer_add_path, result [%d] zipPath [%s] directoryPath [%s]", result, zipPath_.c_str(), directoryPath);
    throw ZipException("mz_stream_open", result);
  }
}

auto ZipArchiver::deletePath(std::string_view const path) const -> void {
  LOGD("deletePath, path [%s]", path);
  (void)path;
}

auto ZipArchiver::containsPath(std::string_view const path) const -> bool {
  LOGD("containsPath, path [%s]", path);
  auto *handle = getZipHandle(zipPath_);
  auto scopedStreamClose = ScopedMzStreamClose(handle);
  auto pathAsString = std::string(path);
  auto result = mz_zip_reader_locate_entry(handle, pathAsString.c_str(), 0);
  return result == MZ_OK;
}
