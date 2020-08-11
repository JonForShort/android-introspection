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
#include <filesystem>
#include <fstream>
#include <memory>
#include <utility>

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

auto getAllEntriesInZipFile(std::string const &pathToArchive) {
  using PathAndFileInfo = std::pair<std::string, unz_file_info64>;
  auto entries = std::vector<PathAndFileInfo>();
  auto openedZipFile = ScopedUnzOpenFile(pathToArchive.c_str());
  if (openedZipFile.get() == nullptr) {
    LOGW("getAllEntriesInZipFile, pathToArchive [{}]", pathToArchive.c_str());
    return entries;
  }
  if (auto result = unzGoToFirstFile(openedZipFile.get()); result == UNZ_OK) {
    do {
      unz_file_info64 fileInfo = {};
      char fileNameInZip[256] = {};
      result = unzGetCurrentFileInfo64(openedZipFile.get(), &fileInfo, fileNameInZip, sizeof(fileNameInZip), nullptr, 0, nullptr, 0);
      if (result == UNZ_OK) {
        entries.push_back(std::make_pair(std::string(fileNameInZip), fileInfo));
      }
      result = unzGoToNextFile(openedZipFile.get());
    } while (UNZ_OK == result);
  }
  return entries;
}

} // namespace

auto ZipArchiver::add(std::istream &source, std::string_view const pathInArchive) const -> void {
  LOGD("add, pathInArchive [{}]", pathInArchive);
  auto const zipFile = openZipFile(zipPath_);
  auto const pathInArchiveString = std::string(pathInArchive);
  if (auto result = zipOpenNewFileInZip_64(zipFile->get(), pathInArchiveString.c_str(), nullptr, nullptr, 0, nullptr, 0, nullptr, 0, 0, false);
      result == ZIP_OK) {
    auto scopedZipCloseFileInZip = ScopedZipCloseFileInZip(zipFile->get());
    writeStreamToOpenZipFile(source, zipFile->get());
  }
}

auto ZipArchiver::files() const -> std::vector<std::string> {
  auto files = std::vector<std::string>();
  for (auto const &entry : getAllEntriesInZipFile(zipPath_)) {
    files.push_back(entry.first);
  }
  return files;
}

auto ZipArchiver::contains(std::string_view pathInArchive) const -> bool {
  LOGD("contains, pathInArchive [{}]", pathInArchive);
  if (auto const zipFile = ScopedUnzOpenFile(zipPath_.c_str()); zipFile.get() != nullptr) {
    auto const pathInArchiveString = std::string(pathInArchive);
    return unzLocateFile(zipFile.get(), pathInArchiveString.c_str(), nullptr) == UNZ_OK;
  } else {
    return false;
  }
}

auto ZipArchiver::extractAll(std::string_view destinationDirectory) const -> void {
  LOGD("extractAll, destinationDirectory [{}]", destinationDirectory);
  namespace fs = std::filesystem;
  if (!fs::exists(destinationDirectory)) {
    LOGD("extractAll, destinationDirectory does not exist; creating directory");
    fs::create_directory(destinationDirectory);
  }
  auto const isPathValid = fs::is_directory(destinationDirectory);
  if (!isPathValid) {
    throw std::logic_error("destinationDirectory must be a directory or must not exist");
  }
  auto const entries = getAllEntriesInZipFile(zipPath_);
  for (auto const &entry : entries) {
    extract(entry.first, destinationDirectory);
  }
}

auto ZipArchiver::extract(std::string_view pathInArchive, std::string_view destinationDirectory) const -> void {
  LOGD("extract, pathInArchive [{}] destinationDirectory [{}]", pathInArchive, destinationDirectory);
  namespace fs = std::filesystem;
  if (!fs::exists(destinationDirectory)) {
    LOGD("extractAll, destinationDirectory does not exist; creating directory");
    fs::create_directory(destinationDirectory);
  }
  auto const isPathValid = fs::is_directory(destinationDirectory);
  if (!isPathValid) {
    throw std::logic_error("path must be a directory or must not exist");
  }
  auto const fileContents = extract(pathInArchive);
  auto const destinationDirectoryString = std::string(destinationDirectory);
  auto const fullPathToExtract = fs::path(destinationDirectoryString) / std::string(pathInArchive);
  auto outputFile = std::ofstream(fullPathToExtract.string(), std::fstream::binary);
  outputFile.write(reinterpret_cast<char const *>(fileContents.data()), sizeof(std::byte) * static_cast<uint32_t>(fileContents.size()));
  outputFile.close();
}

auto ZipArchiver::extract(std::string_view pathInArchive) const -> std::vector<std::byte> {
  LOGD("extract, pathInArchive [{}]", pathInArchive);
  auto const entries = getAllEntriesInZipFile(zipPath_);
  auto pathInZip = std::find_if(entries.cbegin(), entries.cend(),
                                [&pathInArchive = std::as_const(pathInArchive)](auto const &entry) { return entry.first == pathInArchive; });
  if (pathInZip == entries.cend()) {
    throw std::logic_error("path does not exist in archive");
  }
  if (auto const zipFile = ScopedUnzOpenFile(zipPath_.c_str()); zipFile.get() == nullptr) {
    throw std::logic_error("archive does not exist");
  } else {
    if (auto const result = unzLocateFile(zipFile.get(), pathInZip->first.c_str(), nullptr); result != UNZ_OK) {
      throw std::logic_error("path does not exist in archive");
    }
    unz_file_info zipFileInfo;
    if (auto const result = unzGetCurrentFileInfo(zipFile.get(), &zipFileInfo, nullptr, 0, nullptr, 0, nullptr, 0); result != UNZ_OK) {
      throw std::logic_error("unable to get file info in archive");
    }
    auto const openedZipFile = ScopedUnzOpenCurrentFile(zipFile.get());
    if (auto result = openedZipFile.result(); result != MZ_OK) {
      throw std::logic_error("unable to open zip entry in archive");
    }
    auto contents = std::vector<std::byte>(zipFileInfo.uncompressed_size);
    auto const contentsSize = static_cast<uint32_t>(contents.size());
    if (auto const bytesRead = unzReadCurrentFile(zipFile.get(), &contents[0], contentsSize); static_cast<uint32_t>(bytesRead) != contentsSize) {
      throw std::logic_error("unable to read full file in archive");
    }
    return contents;
  }
}
