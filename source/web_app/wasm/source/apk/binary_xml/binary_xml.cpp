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
#include <codecvt>
#include <locale>

#include "attributes_getter_visitor.h"
#include "attributes_setter_visitor.h"
#include "binary_xml.h"
#include "binary_xml_visitor.h"
#include "resource_types.h"
#include "string_xml_visitor.h"
#include "utils/data_stream.h"
#include "utils/log.h"
#include "utils/macros.h"
#include "utils/utils.h"

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

using namespace ai;

using ai::utils::formatString;

using Bytes = std::vector<std::byte>;
using Strings = std::vector<std::string>;
using StringPairs = std::map<std::string, std::string>;
using ElementPointers = std::vector<std::shared_ptr<BinaryXmlElement>>;

struct TraverseContext {

  TraverseContext(Bytes const &content, Strings const &strings) : contentStream_(DataStream(content)), strings_(strings) {}

  auto contentStream() -> DataStream & { return contentStream_; }

  auto strings() -> Strings & { return strings_; }

  auto elements() -> ElementPointers & { return elements_; }

private:
  DataStream contentStream_;

  Strings strings_;

  ElementPointers elements_;
};

auto handleAttributes(DataStream &contentStream, Strings const &strings) {
  StringPairs attributes;
  auto const attributeMarker = contentStream.read<uint32_t>();
  if (attributeMarker != XML_ATTRS_MARKER) {
    LOGW("unexpected attributes marker");
    return attributes;
  }

  auto const attributesCount = contentStream.read<uint32_t>();
  contentStream.skip(sizeof(uint32_t));

  for (auto i{0U}; i < attributesCount; i++) {
    auto const attributeNamespaceIndex = contentStream.read<uint32_t>();
    auto const attributeNameIndex = contentStream.read<uint32_t>();
    auto const attributeValueIndex = contentStream.read<uint32_t>();

    utils::ignore(attributeNamespaceIndex);

    contentStream.skip(sizeof(uint16_t));
    contentStream.skip(sizeof(uint8_t));

    auto const attributeValueType = contentStream.read<uint8_t>();
    auto const attributeResourceId = contentStream.read<uint32_t>();

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

auto handleStartElementTag(TraverseContext &context, BinaryXmlVisitor &visitor) {
  auto &contentStream = context.contentStream();
  auto const &strings = context.strings();

  contentStream.skip(sizeof(uint32_t));
  contentStream.skip(sizeof(uint32_t));

  auto const namespaceStringIndex = contentStream.read<int32_t>();
  auto const namespaceString = namespaceStringIndex >= 0 ? strings[static_cast<uint32_t>(namespaceStringIndex)] : "";

  auto const stringIndex = contentStream.read<int32_t>();
  auto const string = stringIndex >= 0 ? strings[static_cast<uint32_t>(stringIndex)] : "";

  auto const attributes = handleAttributes(contentStream, strings);

  LOGI("start tag [{}] namespace [{}]", string.c_str(), namespaceString.c_str());

  auto element = std::make_shared<StartXmlTagElement>(string, namespaceString, attributes);
  element->accept(visitor);
  context.elements().push_back(element);
}

auto handleEndElementTag(TraverseContext &context, BinaryXmlVisitor &visitor) {
  auto &contentStream = context.contentStream();
  auto const &strings = context.strings();

  contentStream.skip(sizeof(uint32_t));
  contentStream.skip(sizeof(uint32_t));

  auto const namespaceStringIndex = contentStream.read<int32_t>();
  auto const namespaceString = namespaceStringIndex >= 0 ? strings[static_cast<uint32_t>(namespaceStringIndex)] : "";

  auto const stringIndex = contentStream.read<int32_t>();
  auto const string = stringIndex >= 0 ? strings[static_cast<uint32_t>(stringIndex)] : "";

  LOGI("end tag [{}] namespace [{}]", string.c_str(), namespaceString.c_str());

  auto element = std::make_shared<EndXmlTagElement>(string, namespaceString);
  element->accept(visitor);
  context.elements().pop_back();
}

auto handleCDataTag(TraverseContext &context, BinaryXmlVisitor &visitor) {
  auto &contentStream = context.contentStream();
  auto const &strings = context.strings();

  contentStream.skip(sizeof(uint32_t));
  contentStream.skip(sizeof(uint32_t));

  auto const stringIndex = contentStream.read<uint32_t>();
  auto const string = strings[stringIndex];

  contentStream.skip(sizeof(uint32_t));
  contentStream.skip(sizeof(uint32_t));

  LOGI("cdata tag [{}]", string.c_str());

  auto element = std::make_shared<CDataTagElement>(string);
  element->accept(visitor);
}

} // namespace

BinaryXml::BinaryXml(std::vector<std::byte> const &bytes) : content_(std::make_unique<BinaryXmlContent>()) {
  content_->bytes = bytes;
  content_->header = getXmlHeader();
  content_->strings = getStrings();
  content_->offsets = getStringOffsets();
  content_->utf8Encoded = isStringsUtf8Encoded();
}

auto BinaryXml::hasElement(std::string_view elementTag) const -> bool {
  auto const strings = getStrings();
  return std::find(strings.cbegin(), strings.cend(), elementTag) != strings.end();
}

auto BinaryXml::getElementAttributes(std::vector<std::string> elementPath) const -> ElementAttributes {
  ElementAttributes elementAttributes;
  auto visitor = AttributesGetterVisitor(elementPath, elementAttributes);
  traverseXml(visitor);
  return elementAttributes;
}

auto BinaryXml::setElementAttribute(std::vector<std::string> elementPath, std::string_view attributeName, std::string_view attributeValue) const -> void {
  auto visitor = AttributesSetterVisitor(elementPath, attributeName, attributeValue);
  traverseXml(visitor);
}

auto BinaryXml::toStringXml() const -> std::string {
  std::string xml;
  auto visitor = StringXmlVisitor(xml, isStringsUtf8Encoded());
  traverseXml(visitor);
  return xml;
}

auto BinaryXml::toBinaryXml() const -> std::vector<std::byte> {
  return content_->bytes;
}

auto BinaryXml::getXmlHeader() const -> BinaryXmlHeader const * {
  auto xmlHeader = reinterpret_cast<BinaryXmlHeader const *>(content_->bytes.data());
  if (xmlHeader->magicNumber != XML_IDENTIFIER) {
    throw std::logic_error("invalid xml header; missing xml identifier");
  }
  if (xmlHeader->stringTableIdentifier != XML_STRING_TABLE) {
    throw std::logic_error("invalid xml header; missing xml string table");
  }
  return xmlHeader;
}

auto BinaryXml::getStringOffsets() const -> std::vector<std::uint32_t> {
  auto stringOffsets = std::vector<uint32_t>();
  auto stream = DataStream(content_->bytes);
  stream.skip(sizeof(BinaryXmlHeader));
  for (size_t i{0}; i < content_->header->numStrings; i++) {
    auto const offset = stream.read<uint32_t>();
    stringOffsets.push_back(offset);
  }
  return stringOffsets;
}

auto BinaryXml::isStringsUtf8Encoded() const -> bool {
  auto const xmlHeader = content_->header;
  return (xmlHeader->flags & RES_FLAG_UTF8) == RES_FLAG_UTF8;
}

auto BinaryXml::getStrings() const -> Strings {
  auto const stringOffsets = getStringOffsets();
  auto const stringOffsetsInBytes = stringOffsets.size() * sizeof(decltype(stringOffsets)::value_type);
  auto const startStringsOffset = static_cast<uint32_t>(sizeof(BinaryXmlHeader) + stringOffsetsInBytes);
  auto const isUtf8Encoded = isStringsUtf8Encoded();
  auto contentStream = DataStream(content_->bytes);

  std::vector<std::string> strings;
  for (auto const &offset : stringOffsets) {
    if (isUtf8Encoded) {
      contentStream.reset();
      contentStream.skip(startStringsOffset + offset);

      auto stringLength = contentStream.read<uint8_t>();
      auto stringOffset = reinterpret_cast<const char *>(content_->bytes.data() + startStringsOffset + offset + 2);
      auto string = std::string(stringOffset, stringLength);
      strings.push_back(string);
    } else {
      contentStream.reset();
      contentStream.skip(startStringsOffset + offset);

      auto stringLength = contentStream.read<uint16_t>();
      auto stringOffset = reinterpret_cast<const char16_t *>(content_->bytes.data() + startStringsOffset + offset + 2);
      auto string = std::u16string(stringOffset, stringLength);
      std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
      strings.push_back(converter.to_bytes(string));
    }
  }
  return strings;
}

auto BinaryXml::getXmlChunkOffset() const -> uint64_t {
  auto const &bytes = content_->bytes;
  auto const &header = content_->header;
  if (bytes.size() <= header->chunkSize) {
    throw std::logic_error("unable to get chunk size; missing string marker");
  }
  return bytes.size() - (bytes.size() - header->chunkSize);
}

auto BinaryXml::traverseXml(BinaryXmlVisitor &visitor) const -> void {
  auto const xmlChunkOffset = getXmlChunkOffset();
  if (xmlChunkOffset == 0) {
    InvalidXmlTagElement("chunk offset is zero").accept(visitor);
    return;
  }

  auto context = TraverseContext(content_->bytes, getStrings());
  auto &contentStream = context.contentStream();
  contentStream.skip(static_cast<uint32_t>(xmlChunkOffset));

  for (auto tag = contentStream.read<uint16_t>(); tag != RES_XML_END_NAMESPACE_TYPE; tag = contentStream.read<uint16_t>()) {
    auto const headerSize = contentStream.read<uint16_t>();
    auto const chunkSize = contentStream.read<uint32_t>();

    LOGV("traverseXml: tag = [{:d}], headerSize = [{:d}], chunkSize = [{:d}]", tag, headerSize, chunkSize);

    utils::ignore(headerSize);

    switch (tag) {
    case RES_XML_START_NAMESPACE_TYPE: {
      contentStream.skip(chunkSize - 8);
      break;
    }
    case RES_XML_RESOURCE_MAP_TYPE: {
      contentStream.skip(chunkSize - 8);
      break;
    }
    case RES_XML_START_ELEMENT_TYPE: {
      handleStartElementTag(context, visitor);
      break;
    }
    case RES_XML_END_ELEMENT_TYPE: {
      handleEndElementTag(context, visitor);
      break;
    }
    case RES_XML_CDATA_TYPE: {
      handleCDataTag(context, visitor);
      break;
    }
    default: {
      LOGW("traverseXml: skipping unknown tag [{}]", tag);
    }
    }
  }
}
