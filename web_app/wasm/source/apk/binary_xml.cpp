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

#include "binary_xml.h"
#include "resource_types.h"
#include "utils/log.h"

using namespace ai;

namespace {

struct BinaryXmlHeader {
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

} // namespace

auto BinaryXml::readStrings() -> std::vector<std::string> {

  std::vector<std::string> strings;
  auto xmlHeader = reinterpret_cast<BinaryXmlHeader const *>(content_.data());
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
    auto index = sizeof(BinaryXmlHeader) + i * (sizeof(uint32_t));
    auto const offset = readBytesAtIndex<uint32_t>(content_, index);
    stringOffsets.push_back(offset);
  }

  //
  // TODO: Figure out why we can't just use xmlheader->stringsOffset.  Using
  // this will make us 8 bytes short of where strings really start.
  //

  auto const stringOffsetsInBytes = stringOffsets.size() * sizeof(decltype(stringOffsets)::value_type);
  auto const startStringsOffset = sizeof(BinaryXmlHeader) + stringOffsetsInBytes;
  auto const isUtf8Encoded = (xmlHeader->flags & RES_FLAG_UTF8) == RES_FLAG_UTF8;
  for (auto &offset : stringOffsets) {
    if (isUtf8Encoded) {
      auto stringLengthOffset = startStringsOffset + offset;
      auto stringLength = readBytesAtIndex<uint8_t>(content_, stringLengthOffset);
      auto stringOffset = reinterpret_cast<const char *>(content_.data() + startStringsOffset + offset + 2);
      auto string = std::string(stringOffset, stringLength);
      strings.push_back(string);
    } else {
      auto stringLengthOffset = startStringsOffset + offset;
      auto stringLength = readBytesAtIndex<uint16_t>(content_, stringLengthOffset);
      auto stringOffset = reinterpret_cast<const char16_t *>(content_.data() + startStringsOffset + offset + 2);
      auto string = std::u16string(stringOffset, stringLength);
      std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
      strings.push_back(converter.to_bytes(string));
    }
  }
  return strings;
}
