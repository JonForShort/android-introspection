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
#include "zip_archiver.h"
#include "mz.h"
#include "mz_strm.h"
#include "mz_zip.h"
#include "mz_zip_rw.h"
#include "utils/log.h"
#include "utils/utils.h"

#include <filesystem>

using namespace ai;

auto ZipArchiver::createArchive() -> void {
  void *archiveStream = nullptr;
  if (auto result = mz_stream_open(archiveStream, zipPath_.data(), MZ_OPEN_MODE_READWRITE | MZ_OPEN_MODE_CREATE); result != MZ_OK) {
    LOGW("mz_stream_open, unable to create archive [%s]", zipPath_.c_str());
    return;
  }
  if (auto result = mz_stream_close(archiveStream); result != MZ_OK) {
    LOGW("mz_stream_close, unable to close stream [%s]", zipPath_.c_str());
    return;
  }
}

auto ZipArchiver::deleteArchive() -> void { std::filesystem::remove(zipPath_); }

auto ZipArchiver::addFileToArchive(std::string const &fileName, std::string const &filePath) -> void {
  mz_zip_file fileInfo = {};
  fileInfo.creation_date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  fileInfo.modified_date = fileInfo.creation_date;
  fileInfo.accessed_date = fileInfo.creation_date;
  fileInfo.uncompressed_size = static_cast<long>(std::filesystem::file_size(filePath));
  fileInfo.filename = zipPath_.c_str();
  fileInfo.filename_size = static_cast<uint16_t>(fileName.size());
  fileInfo.flag = MZ_ZIP_FLAG_UTF8;
  fileInfo.version_madeby = MZ_HOST_SYSTEM_UNIX << 8 | 63;
  fileInfo.compression_method = MZ_COMPRESS_METHOD_DEFLATE;
  fileInfo.zip64 = MZ_ZIP64_AUTO;

  void *archiveStream = nullptr;
  if (auto result = mz_stream_open(archiveStream, zipPath_.data(), MZ_OPEN_MODE_WRITE); result != MZ_OK) {
    LOGW("mz_stream_open, unable to open stream, result=[%d] zipPath[%s]", result, zipPath_.c_str());
    return;
  }
  if (auto result = mz_zip_entry_write_open(archiveStream, &fileInfo, MZ_COMPRESS_LEVEL_DEFAULT, 0, nullptr); result != MZ_OK) {
    LOGW("mz_zip_entry_write_open, result=[%d] zipPath=[%s] fileName=[%s]", result, zipPath_.c_str(), fileName.c_str());
    return;
  }
  auto const content = std::vector<std::byte>();
  if (auto result = mz_zip_entry_write(archiveStream, &content[0], static_cast<int32_t>(content.size())); result != MZ_OK) {
    LOGW("mz_zip_entry_write, result=[%d] zipPath=[%s] fileName=[%s]", result, zipPath_.c_str(), fileName.c_str());
    return;
  }
  if (auto result = mz_stream_close(archiveStream); result != MZ_OK) {
    LOGW("mz_stream_close, unable to close stream [%s]", zipPath_.c_str());
    return;
  }
}
