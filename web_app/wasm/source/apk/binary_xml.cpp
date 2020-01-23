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
#include <codecvt>
#include <locale>
#include <map>

#include "binary_xml.h"
#include "binary_xml_visitor.h"
#include "resource_types.h"
#include "utils/log.h"
#include "utils/utils.h"

using namespace ai;

using ai::utils::formatString;

using bytes = std::vector<std::byte>;
using strings = std::vector<std::string>;

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

template <typename T, typename U> auto readBytesAtIndex(bytes const &data, U &index) {
  T value = {0};
  memcpy(&value, &data[index], sizeof(value));
  index += sizeof(value);
  return value;
}

auto handleAttributes(bytes const &contents, strings const &strings, uint64_t &contentsOffset) -> std::map<std::string, std::string> {
  std::map<std::string, std::string> attributes;
  auto const attributeMarker = readBytesAtIndex<uint32_t>(contents, contentsOffset);
  if (attributeMarker != XML_ATTRS_MARKER) {
    LOGW("unexpected attributes marker");
    return attributes;
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
    attributes[attributeName] = attributeValue;
  }
  return attributes;
}

auto handleStartElementTag(bytes const &contents, strings const &strings, uint64_t &contentsOffset, BinaryXmlVisitor const &visitor) -> void {
  readBytesAtIndex<uint32_t>(contents, contentsOffset);
  readBytesAtIndex<uint32_t>(contents, contentsOffset);

  auto const namespaceStringIndex = readBytesAtIndex<int32_t>(contents, contentsOffset);
  auto const namespaceString = namespaceStringIndex >= 0 ? strings[static_cast<uint32_t>(namespaceStringIndex)] : "";

  auto const stringIndex = readBytesAtIndex<int32_t>(contents, contentsOffset);
  auto const string = stringIndex >= 0 ? strings[static_cast<uint32_t>(stringIndex)] : "";

  auto const attributes = handleAttributes(contents, strings, contentsOffset);

  LOGI("start tag [{}] namespace [{}]", string.c_str(), namespaceString.c_str());

  StartXmlTagElement(string, attributes).accept(visitor);
}

auto handleEndElementTag(bytes const &contents, strings const &strings, uint64_t &contentsOffset, BinaryXmlVisitor const &visitor) -> void {
  readBytesAtIndex<uint32_t>(contents, contentsOffset);
  readBytesAtIndex<uint32_t>(contents, contentsOffset);

  auto const namespaceStringIndex = readBytesAtIndex<int32_t>(contents, contentsOffset);
  auto const namespaceString = namespaceStringIndex >= 0 ? strings[static_cast<uint32_t>(namespaceStringIndex)] : "";

  auto const stringIndex = readBytesAtIndex<int32_t>(contents, contentsOffset);
  auto const string = stringIndex >= 0 ? strings[static_cast<uint32_t>(stringIndex)] : "";

  LOGI("end tag [{}] namespace [{}]", string.c_str(), namespaceString.c_str());

  EndXmlTagElement(string).accept(visitor);
}

auto handleCDataTag(bytes const &contents, strings const &strings, uint64_t &contentsOffset) -> void {
  readBytesAtIndex<uint32_t>(contents, contentsOffset);
  readBytesAtIndex<uint32_t>(contents, contentsOffset);

  auto stringIndex = readBytesAtIndex<uint32_t>(contents, contentsOffset);
  auto string = strings[stringIndex];

  readBytesAtIndex<uint32_t>(contents, contentsOffset);
  readBytesAtIndex<uint32_t>(contents, contentsOffset);

  LOGD("handling cdata tag [%s]", string.c_str());
}

} // namespace

BinaryXml::BinaryXml(std::vector<std::byte> const &bytes) : content_(std::make_unique<BinaryXmlContent>()) {
  content_->header = getXmlHeader();
  content_->bytes = bytes;
  content_->strings = getStrings();
  content_->stringOffsets = getStringOffsets();
  content_->isUtf8Encoded = isStringsUtf8Encoded();
}

auto BinaryXml::getXmlHeader() const -> BinaryXmlHeader const * {
  auto xmlHeader = reinterpret_cast<BinaryXmlHeader const *>(content_->bytes.data());
  if (xmlHeader->xmlMagicNumber != XML_IDENTIFIER) {
    throw std::logic_error("invalid xml header; missing xml identifier");
  }
  if (xmlHeader->stringTableIdentifier != XML_STRING_TABLE) {
    throw std::logic_error("invalid xml header; missing xml string table");
  }
  return xmlHeader;
}

auto BinaryXml::getStringOffsets() const -> std::vector<std::uint32_t> {
  auto stringOffsets = std::vector<uint32_t>();
  for (size_t i = 0; i < content_->header->numStrings; i++) {
    auto index = sizeof(BinaryXmlHeader) + i * (sizeof(uint32_t));
    auto const offset = readBytesAtIndex<uint32_t>(content_->bytes, index);
    stringOffsets.push_back(offset);
  }
  return stringOffsets;
}

auto BinaryXml::isStringsUtf8Encoded() const -> bool {
  auto const xmlHeader = content_->header;
  return (xmlHeader->flags & RES_FLAG_UTF8) == RES_FLAG_UTF8;
}

auto BinaryXml::getStrings() const -> strings {

  //
  // TODO: Figure out why we can't just use xmlheader->stringsOffset.  Using
  // this will make us 8 bytes short of where strings really start.
  //
  auto const stringOffsets = getStringOffsets();
  auto const stringOffsetsInBytes = stringOffsets.size() * sizeof(decltype(stringOffsets)::value_type);
  auto const startStringsOffset = sizeof(BinaryXmlHeader) + stringOffsetsInBytes;
  auto const isUtf8Encoded = isStringsUtf8Encoded();

  std::vector<std::string> strings;
  for (auto &offset : stringOffsets) {
    if (isUtf8Encoded) {
      auto stringLengthOffset = startStringsOffset + offset;
      auto stringLength = readBytesAtIndex<uint8_t>(content_->bytes, stringLengthOffset);
      auto stringOffset = reinterpret_cast<const char *>(content_->bytes.data() + startStringsOffset + offset + 2);
      auto string = std::string(stringOffset, stringLength);
      strings.push_back(string);
    } else {
      auto stringLengthOffset = startStringsOffset + offset;
      auto stringLength = readBytesAtIndex<uint16_t>(content_->bytes, stringLengthOffset);
      auto stringOffset = reinterpret_cast<const char16_t *>(content_->bytes.data() + startStringsOffset + offset + 2);
      auto string = std::u16string(stringOffset, stringLength);
      std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
      strings.push_back(converter.to_bytes(string));
    }
  }
  return strings;
}

auto BinaryXml::getXmlChunkOffset() const -> uint64_t {
  auto xmlHeader = reinterpret_cast<BinaryXmlHeader const *>(content_->bytes.data());
  if (xmlHeader->xmlMagicNumber != XML_IDENTIFIER) {
    LOGW("unable to get chunk size; compressed xml is invalid");
    return 0;
  }
  if (xmlHeader->stringTableIdentifier != XML_STRING_TABLE) {
    LOGW("unable to get chunk size; missing string marker");
    return 0;
  }
  if (content_->bytes.size() <= xmlHeader->chunkSize) {
    LOGW("unable to get chunk size; missing string marker");
    return 0;
  }
  return content_->bytes.size() - (content_->bytes.size() - xmlHeader->chunkSize);
}

auto BinaryXml::traverseXml(BinaryXmlVisitor const &visitor) const -> void {
  auto const xmlChunkOffset = getXmlChunkOffset();
  if (xmlChunkOffset == 0) {
    InvalidXmlTagElement("chunk offset is zero").accept(visitor);
    return;
  }

  auto currentXmlChunkOffset = xmlChunkOffset;
  auto tag = readBytesAtIndex<uint16_t>(content_->bytes, currentXmlChunkOffset);

  do {
    auto const headerSize = readBytesAtIndex<uint16_t>(content_->bytes, currentXmlChunkOffset);
    auto const chunkSize = readBytesAtIndex<uint32_t>(content_->bytes, currentXmlChunkOffset);
    auto const strings = getStrings();

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
      handleStartElementTag(content_->bytes, strings, currentXmlChunkOffset, visitor);
      break;
    }
    case RES_XML_END_ELEMENT_TYPE: {
      handleEndElementTag(content_->bytes, strings, currentXmlChunkOffset, visitor);
      break;
    }
    case RES_XML_CDATA_TYPE: {
      handleCDataTag(content_->bytes, strings, currentXmlChunkOffset);
      break;
    }
    default: {
      LOGW("skipping unknown tag [%d]", tag);
    }
    }
    tag = readBytesAtIndex<uint16_t>(content_->bytes, currentXmlChunkOffset);
  } while (tag != RES_XML_END_NAMESPACE_TYPE);
}
