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
#ifndef ANDROID_INTROSPECTION_APK_BINARY_XML_H_
#define ANDROID_INTROSPECTION_APK_BINARY_XML_H_

#include <cstdint>
#include <string>
#include <vector>

#include "binary_xml_visitor.h"

namespace ai {

class BinaryXml {

  std::vector<std::byte> const content_;

  auto getXmlChunkOffset() const -> uint64_t;

public:
  BinaryXml(std::vector<std::byte> const &content) : content_(content) {}

  auto readStrings() const -> std::vector<std::string>;

  auto traverseXml(BinaryXmlVisitor const &visitor) const -> void;
};

} // namespace ai

#endif /* ANDROID_INTROSPECTION_APK_APK_PARSER_H_ */
