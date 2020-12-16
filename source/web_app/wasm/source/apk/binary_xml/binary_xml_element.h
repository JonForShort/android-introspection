//
// MIT License
//
// Copyright 2020
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
#ifndef ANDROID_INTROSPECTION_APK_BINARY_XML_ELEMENT_H_
#define ANDROID_INTROSPECTION_APK_BINARY_XML_ELEMENT_H_

#include <map>
#include <string>

namespace ai {

class BinaryXmlVisitor;

class BinaryXmlElement {
public:
  virtual ~BinaryXmlElement();

  virtual auto tag() const -> std::string = 0;
};

class StartXmlTagElement final : public BinaryXmlElement {
  std::string const tag_;

  std::string const nameSpace_;

  std::map<std::string, std::string> attributes_;

public:
  StartXmlTagElement(std::string const tag, std::string const nameSpace, std::map<std::string, std::string> attributes)
      : tag_(tag), nameSpace_(nameSpace), attributes_(attributes) {}

  ~StartXmlTagElement() override;

  auto tag() const -> std::string override;

  auto nameSpace() const -> std::string;

  auto attributes() const -> std::map<std::string, std::string>;

  auto accept(BinaryXmlVisitor &visitor) const -> void;
};

class EndXmlTagElement final : public BinaryXmlElement {

  std::string const tag_;

  std::string const nameSpace_;

public:
  EndXmlTagElement(std::string const tag, std::string const nameSpace) : tag_(tag), nameSpace_(nameSpace) {}

  ~EndXmlTagElement() override;

  auto tag() const -> std::string override;

  auto nameSpace() const -> std::string;

  auto accept(BinaryXmlVisitor &visitor) const -> void;
};

class CDataTagElement final : public BinaryXmlElement {
  std::string const tag_;

public:
  CDataTagElement(std::string const tag) : tag_(tag) {}

  ~CDataTagElement() override;

  auto tag() const -> std::string override;

  auto accept(BinaryXmlVisitor &visitor) const -> void;
};

class InvalidXmlTagElement final : public BinaryXmlElement {
  std::string const error_;

public:
  InvalidXmlTagElement(std::string const error) : error_(error) {}

  ~InvalidXmlTagElement() override;

  auto tag() const -> std::string override;

  auto error() const -> std::string;

  auto accept(BinaryXmlVisitor &visitor) const -> void;
};

} // namespace ai

#endif /* ANDROID_INTROSPECTION_APK_BINARY_XML_ELEMENT_H_ */
