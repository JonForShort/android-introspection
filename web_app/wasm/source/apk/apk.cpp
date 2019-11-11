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
#include <algorithm>
#include <codecvt>
#include <cstdint>
#include <locale>
#include <string>
#include <vector>

#include "apk/apk.h"
#include "resource_types.h"
#include "utils/log.h"
#include "zip.h"

using namespace ai;

//
// Implementation used the following resources.
//
// https://android.googlesource.com/platform/frameworks/base/+/master/libs/androidfw/include/androidfw/ResourceTypes.h
// https://android.googlesource.com/platform/frameworks/base/+/master/libs/androidfw/ResourceTypes.cpp
// https://github.com/google/android-classyshark/blob/master/ClassySharkWS/src/com/google/classyshark/silverghost/translator/xml/XmlDecompressor.java
//
// Structure of a binary xml file (i.e. compressed AndroidManifest.xml) is as
// follows.
//
// -----------------------------
// [Header]
// -----------------------------
// [String Offsets]
// -----------------------------
// [Strings]
// -----------------------------
// [Chunk]
// -----------------------------
//

namespace {

struct CompressedAndroidManifestHeader {
  uint32_t xmlMagicNumber;
  uint32_t reservedBytes;
  uint16_t stringTableIdentifier;
  uint16_t headerSize;
  uint32_t chunkSize;
  uint32_t numStrings;
  uint32_t numStyles;
  uint32_t flags;
  uint32_t stringsOffset;
  uint32_t stylesOffset;
};

struct UnzOpenFile {

  explicit UnzOpenFile(const char *szFileName) : zip_(unzOpen(szFileName)) {}

  ~UnzOpenFile() { unzClose(zip_); }

  auto get() const -> unzFile { return zip_; }

private:
  unzFile zip_;
};

auto getZipFileNames(const char *szFileName) -> std::vector<std::string> {
  std::vector<std::string> fileNames;
  auto const zip = UnzOpenFile(szFileName);
  if (zip.get() == nullptr) {
    LOGW("unable to open zip file [%s]", szFileName);
    return fileNames;
  }
  unz_global_info info;
  if (unzGetGlobalInfo(zip.get(), &info) != MZ_OK) {
    LOGW("unable to get global zip info [%s]", szFileName);
    return fileNames;
  }
  if (unzGoToFirstFile(zip.get()) != UNZ_OK) {
    LOGW("unable to get first zip file in archive [%s]", szFileName);
    return fileNames;
  }
  do {
    unz_file_info zipFileInfo;
    char szFilename[BUFSIZ] = {0};
    if (UNZ_OK == unzGetCurrentFileInfo(zip.get(), &zipFileInfo, szFilename, sizeof(szFilename), nullptr, 0, nullptr, 0)) {
      auto fileName = std::string(szFilename);
      fileNames.emplace_back(fileName);
    }
  } while (UNZ_OK == unzGoToNextFile(zip.get()));
  return fileNames;
}

auto getZipContents(const char *szFileName, const char *szZipFileName) -> std::vector<uint8_t> {
  auto contents = std::vector<uint8_t>();
  auto const zip = UnzOpenFile(szFileName);
  if (zip.get() == nullptr) {
    LOGW("unable to open zip file [%s]", szFileName);
    return contents;
  }
  if (unzLocateFile(zip.get(), szZipFileName, nullptr) != MZ_OK) {
    LOGW("unable to locate zip file [%s] [%s]", szFileName, szZipFileName);
    return contents;
  }
  unz_file_info zipFileInfo;
  if (unzGetCurrentFileInfo(zip.get(), &zipFileInfo, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK) {
    LOGW("unable to get current file info [%s] [%s]", szFileName, szZipFileName);
    return contents;
  }
  if (unzOpenCurrentFile(zip.get()) != MZ_OK) {
    LOGW("unable to open current file [%s] [%s]", szFileName, szZipFileName);
    return contents;
  }
  contents.resize(zipFileInfo.uncompressed_size);
  unzReadCurrentFile(zip.get(), &contents[0], static_cast<uint32_t>(contents.size()));
  return contents;
}

template <typename... Args> std::string formatString(const char *format, Args... args) {
  auto size = static_cast<uint64_t>(snprintf(nullptr, 0, format, args...) + 1);
  std::unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format, args...);
  return std::string(buf.get(), buf.get() + size - 1);
}

template <typename T, typename U> auto readBytesAtIndex(std::vector<uint8_t> const &data, U &index) {
  T value = {0};
  memcpy(&value, &data[index], sizeof(value));
  index += sizeof(value);
  return value;
}

auto getStringsFromCompressedAndroidManifest(std::vector<uint8_t> const &contents) -> std::vector<std::string> {

  std::vector<std::string> strings;
  auto xmlHeader = reinterpret_cast<CompressedAndroidManifestHeader const *>(contents.data());
  if (xmlHeader->xmlMagicNumber != XML_IDENTIFIER) {
    LOGW("unable to get strings; compressed xml is invalid");
    return strings;
  }
  if (xmlHeader->stringTableIdentifier != XML_STRING_TABLE) {
    LOGW("unable to get strings; missing string marker");
    return strings;
  }

  auto stringOffsets = std::vector<uint32_t>();
  for (size_t i = 0; i < xmlHeader->numStrings; i++) {
    auto index = sizeof(CompressedAndroidManifestHeader) + i * (sizeof(uint32_t));
    auto const offset = readBytesAtIndex<uint32_t>(contents, index);
    stringOffsets.push_back(offset);
  }

  //
  // TODO: Figure out why we can't just use xmlheader->stringsOffset.  Using
  // this will make us 8 bytes short of where strings really start.
  //

  auto const stringOffsetsInBytes = stringOffsets.size() * sizeof(decltype(stringOffsets)::value_type);
  auto const startStringsOffset = sizeof(CompressedAndroidManifestHeader) + stringOffsetsInBytes;
  auto const isUtf8Encoded = (xmlHeader->flags & RES_FLAG_UTF8) == RES_FLAG_UTF8;
  for (auto &offset : stringOffsets) {
    if (isUtf8Encoded) {
      auto stringLengthOffset = startStringsOffset + offset;
      auto stringLength = readBytesAtIndex<uint8_t>(contents, stringLengthOffset);
      auto stringOffset = reinterpret_cast<const char *>(contents.data() + startStringsOffset + offset + 2);
      auto string = std::string(stringOffset, stringLength);
      strings.push_back(string);
    } else {
      auto stringLengthOffset = startStringsOffset + offset;
      auto stringLength = readBytesAtIndex<uint16_t>(contents, stringLengthOffset);
      auto stringOffset = reinterpret_cast<const char16_t *>(contents.data() + startStringsOffset + offset + 2);
      auto string = std::u16string(stringOffset, stringLength);
      std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
      strings.push_back(converter.to_bytes(string));
    }
  }

  return strings;
}

auto getXmlChunkOffset(std::vector<uint8_t> const &contents) -> uint64_t {
  auto xmlHeader = reinterpret_cast<CompressedAndroidManifestHeader const *>(contents.data());
  if (xmlHeader->xmlMagicNumber != XML_IDENTIFIER) {
    LOGW("unable to get chunk size; compressed xml is invalid");
    return 0;
  }
  if (xmlHeader->stringTableIdentifier != XML_STRING_TABLE) {
    LOGW("unable to get chunk size; missing string marker");
    return 0;
  }
  if (contents.size() <= xmlHeader->chunkSize) {
    LOGW("unable to get chunk size; missing string marker");
    return 0;
  }
  return contents.size() - (contents.size() - xmlHeader->chunkSize);
}

auto handleAttributes(std::vector<uint8_t> const &contents, std::vector<std::string> const &strings, uint64_t &contentsOffset) -> void {
  auto const attributeMarker = readBytesAtIndex<uint32_t>(contents, contentsOffset);
  if (attributeMarker != XML_ATTRS_MARKER) {
    LOGW("unexpected attributes marker");
    return;
  }

  auto const attributesCount = readBytesAtIndex<uint32_t>(contents, contentsOffset);
  readBytesAtIndex<uint32_t>(contents, contentsOffset);

  for (auto i = 0U; i < attributesCount; i++) {
    /* auto const attributeNamespaceIndex = */ readBytesAtIndex<uint32_t>(contents, contentsOffset);
    auto const attributeNameIndex = readBytesAtIndex<uint32_t>(contents, contentsOffset);
    auto const attributeValueIndex = readBytesAtIndex<uint32_t>(contents, contentsOffset);

    readBytesAtIndex<uint16_t>(contents, contentsOffset);
    readBytesAtIndex<uint8_t>(contents, contentsOffset);

    auto const attributeValueType = readBytesAtIndex<uint8_t>(contents, contentsOffset);
    auto const attributeResourceId = readBytesAtIndex<uint32_t>(contents, contentsOffset);

    auto attributeName = strings[attributeNameIndex];
    if (attributeName.empty()) {
      LOGW("unexpected empty attribute name");
      continue;
    }
    std::string attributeValue;
    switch (attributeValueType) {
    case TYPE_NULL: {
      attributeValue = attributeResourceId == 0 ? "<undefined>" : "<empty>";
      break;
    }
    case TYPE_REFERENCE: {
      attributeValue = formatString("@res/0x%08X", attributeResourceId);
      break;
    }
    case TYPE_ATTRIBUTE: {
      attributeValue = formatString("@attr/0x%08X", attributeResourceId);
      break;
    }
    case TYPE_STRING: {
      attributeValue = strings[attributeValueIndex];
      break;
    }
    case TYPE_FLOAT: {
      break;
    }
    case TYPE_DIMENSION: {
      break;
    }
    case TYPE_FRACTION: {
      break;
    }
    case TYPE_DYNAMIC_REFERENCE: {
      attributeValue = formatString("@dyn/0x%08X", attributeResourceId);
      break;
    }
    case TYPE_INT_DEC: {
      attributeValue = formatString("%d", attributeResourceId);
      break;
    }
    case TYPE_INT_HEX: {
      attributeValue = formatString("0x%08X", attributeResourceId);
      break;
    }
    case TYPE_INT_BOOLEAN: {
      attributeValue = attributeResourceId == RES_VALUE_TRUE ? "true" : attributeResourceId == RES_VALUE_FALSE ? "false" : "unknown";
      break;
    }
    default: {
      attributeValue = "unknown";
      break;
    }
    }
    LOGD("handling attribute with value [%s]", attributeValue.c_str());
  }
}

auto handleStartElementTag(std::vector<uint8_t> const &contents, std::vector<std::string> const &strings, uint64_t &contentsOffset) -> void {
  readBytesAtIndex<uint32_t>(contents, contentsOffset);
  readBytesAtIndex<uint32_t>(contents, contentsOffset);

  auto const namespaceStringIndex = readBytesAtIndex<int32_t>(contents, contentsOffset);
  auto const namespaceString = namespaceStringIndex >= 0 ? strings[namespaceStringIndex] : "";

  auto const stringIndex = readBytesAtIndex<int32_t>(contents, contentsOffset);
  auto const string = stringIndex >= 0 ? strings[stringIndex] : "";

  handleAttributes(contents, strings, contentsOffset);

  LOGD("handling start tag [%s] namespace [%s]", string.c_str(), namespaceString.c_str());
}

auto handleElementElementTag(std::vector<uint8_t> const &contents, std::vector<std::string> const &strings, uint64_t &contentsOffset) -> void {
  readBytesAtIndex<uint32_t>(contents, contentsOffset);
  readBytesAtIndex<uint32_t>(contents, contentsOffset);

  auto const namespaceStringIndex = readBytesAtIndex<int32_t>(contents, contentsOffset);
  auto const namespaceString = namespaceStringIndex >= 0 ? strings[namespaceStringIndex] : "";

  auto const stringIndex = readBytesAtIndex<int32_t>(contents, contentsOffset);
  auto const string = stringIndex >= 0 ? strings[stringIndex] : "";

  LOGD("handling start tag [%s] namespace [%s]", string.c_str(), namespaceString.c_str());
}

auto handleCDataTag(std::vector<uint8_t> const &contents, std::vector<std::string> const &strings, uint64_t &contentsOffset) -> void {
  readBytesAtIndex<uint32_t>(contents, contentsOffset);
  readBytesAtIndex<uint32_t>(contents, contentsOffset);

  auto stringIndex = readBytesAtIndex<uint32_t>(contents, contentsOffset);
  auto string = strings[stringIndex];

  readBytesAtIndex<uint32_t>(contents, contentsOffset);
  readBytesAtIndex<uint32_t>(contents, contentsOffset);

  LOGD("handling cdata tag [%s]", string.c_str());
}
} // namespace

auto apk::makeApkDebuggable(const char *apkPath) -> bool {
  auto const fileNames = getZipFileNames(apkPath);
  auto const containsAndroidManifest = std::find(fileNames.begin(), fileNames.end(), "AndroidManifest.xml") != fileNames.end();
  if (!containsAndroidManifest) {
    LOGD("unable to find AndroidManifest.xml in [%s]", apkPath);
    return false;
  }
  auto const contents = getZipContents(apkPath, "AndroidManifest.xml");
  if (contents.empty()) {
    LOGW("unable to read AndroidManifest.xml in [%s]", apkPath);
    return false;
  }
  auto const strings = getStringsFromCompressedAndroidManifest(contents);
  if (strings.empty()) {
    LOGW("unable to parse strings from AndroidManifest.xml in [%s]", apkPath);
    return false;
  }
  if (std::find(strings.begin(), strings.end(), "application") == strings.end()) {
    LOGW("unable to find application tag in AndroidManifest.xml in [%s]", apkPath);
    return false;
  }
  auto const xmlChunkOffset = getXmlChunkOffset(contents);
  if (xmlChunkOffset == 0) {
    LOGW("unable to determine chunk offset in AndroidManifest.xml in [%s]", apkPath);
    return false;
  }

  auto currentXmlChunkOffset = xmlChunkOffset;
  auto tag = readBytesAtIndex<uint16_t>(contents, currentXmlChunkOffset);

  do {
    auto const headerSize = readBytesAtIndex<uint16_t>(contents, currentXmlChunkOffset);
    auto const chunkSize = readBytesAtIndex<uint32_t>(contents, currentXmlChunkOffset);

    LOGD("makeApkDebuggable: tag = [%d], headerSize = [%d], chunkSize = [%d]\n", tag, headerSize, chunkSize);

    switch (tag) {
    case RES_XML_START_NAMESPACE_TYPE: {
      currentXmlChunkOffset += chunkSize - 8;
      break;
    }
    case RES_XML_RESOURCE_MAP_TYPE: {
      currentXmlChunkOffset += chunkSize - 8;
      break;
    }
    case RES_XML_START_ELEMENT_TYPE: {
      handleStartElementTag(contents, strings, currentXmlChunkOffset);
      break;
    }
    case RES_XML_END_ELEMENT_TYPE: {
      handleElementElementTag(contents, strings, currentXmlChunkOffset);
      break;
    }
    case RES_XML_CDATA_TYPE: {
      handleCDataTag(contents, strings, currentXmlChunkOffset);
      break;
    }
    default: {
      LOGW("skipping unknown tag [%d]\n", tag);
    }
    }
    tag = readBytesAtIndex<uint16_t>(contents, currentXmlChunkOffset);
  } while (tag != RES_XML_END_NAMESPACE_TYPE);

  return true;
}
