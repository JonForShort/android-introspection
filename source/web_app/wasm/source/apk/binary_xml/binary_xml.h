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
#ifndef ANDROID_INTROSPECTION_APK_BINARY_XML_H_
#define ANDROID_INTROSPECTION_APK_BINARY_XML_H_

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "binary_xml_visitor.h"

namespace ai {

class BinaryXml {
public:
  using ElementAttributes = std::map<std::string, std::string>;

  explicit BinaryXml(std::vector<std::byte> const &bytes);

  auto hasElement(std::string_view elementTag) const -> bool;

  auto getElementAttributes(std::vector<std::string> elementPath) const -> ElementAttributes;

  auto setElementAttribute(std::vector<std::string> elementPath, std::string_view attributeName, std::string_view attributeValue) const -> void;

  auto toStringXml() const -> std::string;

  auto toBinaryXml() const -> std::vector<std::byte>;

private:
  struct BinaryXmlHeader {

    uint32_t magicNumber;

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

  struct BinaryXmlContent {

    BinaryXmlHeader const *header;

    std::vector<std::byte> bytes;

    std::vector<std::string> strings;

    std::vector<uint32_t> offsets;

    bool utf8Encoded;
  };

  auto traverseXml(BinaryXmlVisitor &visitor) const -> void;

  auto getXmlHeader() const -> BinaryXmlHeader const *;

  auto getXmlChunkOffset() const -> uint64_t;

  auto getStrings() const -> std::vector<std::string>;

  auto getStringOffsets() const -> std::vector<std::uint32_t>;

  auto isStringsUtf8Encoded() const -> bool;

  std::unique_ptr<BinaryXmlContent> content_;
};

} // namespace ai

#endif /* ANDROID_INTROSPECTION_APK_BINARY_XML_H_ */
