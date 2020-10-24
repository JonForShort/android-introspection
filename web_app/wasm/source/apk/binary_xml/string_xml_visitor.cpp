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
#include "string_xml_visitor.h"
#include "utils/macros.h"

using namespace ai;

StringXmlVisitor::StringXmlVisitor(std::string &xml, bool const isStringsUtf8Encoded) : isStringsUtf8Encoded_(isStringsUtf8Encoded), xml_(xml) {
  xml_ += getAndroidManifestHeader();
}

auto StringXmlVisitor::visit(StartXmlTagElement const &element) -> void {
  xml_ += "<";
  xml_ += element.tag();
  xml_ += " ";
  for (auto const &[attributeName, attributeValue] : element.attributes()) {
    xml_ += attributeName;
    xml_ += "=";
    xml_ += "\"" + attributeValue + "\" ";
  }
  xml_ += ">";
  depth_++;
}

auto StringXmlVisitor::visit(EndXmlTagElement const &element) -> void {
  xml_ += "</ ";
  xml_ += element.tag();
  xml_ += " >";
  depth_--;
}

auto StringXmlVisitor::visit(InvalidXmlTagElement const &element) -> void { utils::ignore(element); }

auto StringXmlVisitor::visit(CDataTagElement const &element) -> void { utils::ignore(element); }

auto StringXmlVisitor::getAndroidManifestHeader() const -> char const * {
  return isStringsUtf8Encoded_ ? "<?xml version=\"1.0\" encoding=\"utf-8\"?>" : "<?xml version=\"1.0\" encoding=\"utf-16\"?>";
}
