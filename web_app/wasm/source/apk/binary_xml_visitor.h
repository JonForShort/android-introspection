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
#ifndef ANDROID_INTROSPECTION_APK_BINARY_XML_VISITOR_H_
#define ANDROID_INTROSPECTION_APK_BINARY_XML_VISITOR_H_

#include <map>
#include <string>

namespace ai {

class BinaryXmlElement {
public:
  virtual ~BinaryXmlElement() = default;
};

class StartXmlTagElement final : public BinaryXmlElement {
  std::string const tag_;

  std::map<std::string, std::string> attributes_;

public:
  StartXmlTagElement(std::string const tag, std::map<std::string, std::string> attributes) : tag_(tag), attributes_(attributes) {}

  ~StartXmlTagElement() override = default;

  auto tag() const -> std::string { return tag_; }

  auto attributes() const -> std::map<std::string, std::string> { return attributes_; }
};

class EndXmlTagElement final : public BinaryXmlElement {
  std::string const tag_;

public:
  EndXmlTagElement(std::string const tag) : tag_(tag) {}

  ~EndXmlTagElement() override = default;

  auto tag() const -> std::string { return tag_; }
};

class InvalidXmlTagElement final : public BinaryXmlElement {
  std::string const error_;

public:
  InvalidXmlTagElement(std::string const error) : error_(error) {}

  ~InvalidXmlTagElement() override = default;

  auto error() const -> std::string { return error_; }
};

class BinaryXmlVisitor {
public:
  virtual ~BinaryXmlVisitor() = default;

  virtual auto visit(StartXmlTagElement const &element) const -> void = 0;

  virtual auto visit(EndXmlTagElement const &element) const -> void = 0;

  virtual auto visit(InvalidXmlTagElement const &element) const -> void = 0;
};

} // namespace ai

#endif /* ANDROID_INTROSPECTION_APK_BINARY_XML_VISITOR_H_ */
