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
#include <zip.h>

#include "utils/log.h"
#include "utils/utils.h"

#include "apk/apk.h"
#include "apk_parser.h"
#include "binary_xml.h"
#include "resource_types.h"

using namespace ai;

using ai::utils::formatString;

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

template <typename T, typename U> auto readBytesAtIndex(std::vector<std::byte> const &data, U &index) {
  T value = {0};
  memcpy(&value, &data[index], sizeof(value));
  index += sizeof(value);
  return value;
}

auto getXmlChunkOffset(std::vector<std::byte> const &contents) -> uint64_t {
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

auto handleAttributes(std::vector<std::byte> const &contents, std::vector<std::string> const &strings, uint64_t &contentsOffset) -> void {
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
    LOGI("  attribute value [{}]", attributeValue.c_str());
  }
}

auto handleStartElementTag(std::vector<std::byte> const &contents, std::vector<std::string> const &strings, uint64_t &contentsOffset) -> void {
  readBytesAtIndex<uint32_t>(contents, contentsOffset);
  readBytesAtIndex<uint32_t>(contents, contentsOffset);

  auto const namespaceStringIndex = readBytesAtIndex<int32_t>(contents, contentsOffset);
  auto const namespaceString = namespaceStringIndex >= 0 ? strings[static_cast<uint32_t>(namespaceStringIndex)] : "";

  auto const stringIndex = readBytesAtIndex<int32_t>(contents, contentsOffset);
  auto const string = stringIndex >= 0 ? strings[static_cast<uint32_t>(stringIndex)] : "";

  handleAttributes(contents, strings, contentsOffset);

  LOGI("start tag [{}] namespace [{}]", string.c_str(), namespaceString.c_str());
}

auto handleEndElementTag(std::vector<std::byte> const &contents, std::vector<std::string> const &strings, uint64_t &contentsOffset) -> void {
  readBytesAtIndex<uint32_t>(contents, contentsOffset);
  readBytesAtIndex<uint32_t>(contents, contentsOffset);

  auto const namespaceStringIndex = readBytesAtIndex<int32_t>(contents, contentsOffset);
  auto const namespaceString = namespaceStringIndex >= 0 ? strings[static_cast<uint32_t>(namespaceStringIndex)] : "";

  auto const stringIndex = readBytesAtIndex<int32_t>(contents, contentsOffset);
  auto const string = stringIndex >= 0 ? strings[static_cast<uint32_t>(stringIndex)] : "";

  LOGI("end tag [{}] namespace [{}]", string.c_str(), namespaceString.c_str());
}

auto handleCDataTag(std::vector<std::byte> const &contents, std::vector<std::string> const &strings, uint64_t &contentsOffset) -> void {
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
  auto const apkParser = ai::ApkParser(apkPath);
  auto const fileNames = apkParser.getFileNames();
  auto const containsAndroidManifest = std::find(fileNames.begin(), fileNames.end(), "AndroidManifest.xml") != fileNames.end();
  if (!containsAndroidManifest) {
    LOGD("unable to find AndroidManifest.xml in [%s]", apkPath);
    return false;
  }
  auto const contents = apkParser.getFileContents("AndroidManifest.xml");
  if (contents.empty()) {
    LOGW("unable to read AndroidManifest.xml in [%s]", apkPath);
    return false;
  }
  auto binaryXml = BinaryXml(contents);
  auto const strings = binaryXml.readStrings();
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

    LOGV("makeApkDebuggable: tag = [{:d}], headerSize = [{:d}], chunkSize = [{:d}]", tag, headerSize, chunkSize);

    (void)headerSize;

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
      handleEndElementTag(contents, strings, currentXmlChunkOffset);
      break;
    }
    case RES_XML_CDATA_TYPE: {
      handleCDataTag(contents, strings, currentXmlChunkOffset);
      break;
    }
    default: {
      LOGW("skipping unknown tag [%d]", tag);
    }
    }
    tag = readBytesAtIndex<uint16_t>(contents, currentXmlChunkOffset);
  } while (tag != RES_XML_END_NAMESPACE_TYPE);

  return true;
}
