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

#include "scoped_minizip.h"
#include "utils/log.h"
#include "utils/utils.h"
#include "zip.h"
#include "zip_archiver.h"

using namespace ai;
using namespace ai::minizip;

auto ZipArchiver::addPath(std::istream &source, std::string_view path) const -> void {
  LOGD("addPath, path [%s]", path);
  auto const fileExists = std::filesystem::exists(zipPath_);
  auto const zipMode = fileExists ? APPEND_STATUS_ADDINZIP : APPEND_STATUS_CREATE;
  auto const zipFile = ScopedZipOpen(zipPath_.c_str(), zipMode);
  auto const pathString = std::string(path);
  if (auto result = zipOpenNewFileInZip_64(zipFile.get(), pathString.c_str(), nullptr, nullptr, 0, nullptr, 0, nullptr, 0, 0, false); result == ZIP_OK) {
    std::vector<char> readBuffer;
    auto constexpr readBufferSize = 0xFF;
    readBuffer.resize(readBufferSize);
    size_t readCount = 0;
    do {
      source.read(readBuffer.data(), static_cast<uint32_t>(readBuffer.size()));
      readCount = static_cast<size_t>(source.gcount());
      if (readCount < readBuffer.size() && !source.eof() && !source.good()) {
        result = ZIP_ERRNO;
      } else if (readCount > 0) {
        result = zipWriteInFileInZip(zipFile.get(), readBuffer.data(), static_cast<uint32_t>(readCount));
      }
    } while ((result == ZIP_OK) && (readCount > 0));
    zipCloseFileInZip64(zipFile.get());
  }
}

auto ZipArchiver::containsPath(std::string_view path) const -> bool {
  LOGD("containsPath, path [%s]", path);
  if (auto const zipFile = ScopedUnzOpenFile(zipPath_.c_str()); zipFile.get() != nullptr) {
    auto const pathString = std::string(path);
    return unzLocateFile(zipFile.get(), pathString.c_str(), nullptr) == UNZ_OK;
  } else {
    return false;
  }
}
