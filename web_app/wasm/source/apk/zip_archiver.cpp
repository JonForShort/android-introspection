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
#include <memory>

#include "scoped_minizip.h"
#include "utils/log.h"
#include "utils/utils.h"
#include "zip.h"
#include "zip_archiver.h"

using namespace ai;
using namespace ai::minizip;

namespace {

auto openZipFile(std::string const &path) {
  auto fileExists = std::filesystem::exists(path);
  auto zipMode = fileExists ? APPEND_STATUS_ADDINZIP : APPEND_STATUS_CREATE;
  auto openedZipFile = std::make_unique<ScopedZipOpen>(path.c_str(), zipMode);
  if (openedZipFile->get() == nullptr) {
    throw std::logic_error("unable to open zip file");
  }
  return openedZipFile;
}

auto writeStreamToOpenZipFile(std::istream &source, zipFile const zipFile) {
  auto result = ZIP_OK;
  std::vector<char> readBuffer;
  readBuffer.resize(BUFSIZ);
  size_t readCount = 0;
  do {
    source.read(readBuffer.data(), static_cast<uint32_t>(readBuffer.size()));
    readCount = static_cast<size_t>(source.gcount());
    if (readCount < readBuffer.size() && !source.eof() && !source.good()) {
      result = ZIP_ERRNO;
    } else if (readCount > 0) {
      result = zipWriteInFileInZip(zipFile, readBuffer.data(), static_cast<uint32_t>(readCount));
    }
  } while ((result == ZIP_OK) && (readCount > 0));
}

auto getAllEntriesInZipFile(std::string const &path) {
  auto entries = std::vector<unz_file_info64>();
  auto openedZipFile = ScopedUnzOpenFile(path.c_str());
  if (openedZipFile.get() == nullptr) {
    LOGW("getAllEntriesInZipFile, path [%s]", path.c_str());
    return entries;
  }
  if (auto result = unzGoToFirstFile(openedZipFile.get()); result == UNZ_OK) {
    do {
      unz_file_info64 fileInfo = {};
      char fileNameInZip[256] = {};
      result = unzGetCurrentFileInfo64(openedZipFile.get(), &fileInfo, fileNameInZip, sizeof(fileNameInZip), nullptr, 0, nullptr, 0);
      if (result == UNZ_OK) {
        entries.push_back(fileInfo);
      }
      result = unzGoToNextFile(openedZipFile.get());
    } while (UNZ_OK == result);
  }
  return entries;
}

} // namespace

auto ZipArchiver::add(std::istream &source, std::string_view const path) const -> void {
  LOGD("addPath, path [%s]", path);
  auto const zipFile = openZipFile(zipPath_);
  auto const pathString = std::string(path);
  if (auto result = zipOpenNewFileInZip_64(zipFile->get(), pathString.c_str(), nullptr, nullptr, 0, nullptr, 0, nullptr, 0, 0, false); result == ZIP_OK) {
    auto scopedZipCloseFileInZip = ScopedZipCloseFileInZip(zipFile->get());
    writeStreamToOpenZipFile(source, zipFile->get());
  }
}

auto ZipArchiver::contains(std::string_view path) const -> bool {
  LOGD("containsPath, path [%s]", path);
  if (auto const zipFile = ScopedUnzOpenFile(zipPath_.c_str()); zipFile.get() != nullptr) {
    auto const pathString = std::string(path);
    return unzLocateFile(zipFile.get(), pathString.c_str(), nullptr) == UNZ_OK;
  } else {
    return false;
  }
}

auto ZipArchiver::extractAll(std::string_view path) const -> void {
  LOGD("extractAll, path [%s]", path);
  using namespace std::filesystem;
  auto const isPathValid = is_directory(path) or !exists(path);
  if (!isPathValid) {
    throw std::logic_error("path must be a directory or not exists");
  }
  auto const entries = getAllEntriesInZipFile(zipPath_);
}

auto ZipArchiver::extract(std::string_view fileToExtract, std::string_view path) const -> void {
  LOGD("extract, fileToExtract [%s] path [%s]", fileToExtract, path);
  using namespace std::filesystem;
  auto const isPathValid = is_regular_file(path) or !exists(path);
  if (!isPathValid) {
    throw std::logic_error("path must be a file or not exists");
  }
  auto const entries = getAllEntriesInZipFile(zipPath_);
  auto const fileExistsInZip = std::find(entries.cbegin(), entries.cend(), fileToExtract) != entries.cend();
  if (fileExistsInZip) {
    LOGD("extract, file exists in archive");
  }
}
