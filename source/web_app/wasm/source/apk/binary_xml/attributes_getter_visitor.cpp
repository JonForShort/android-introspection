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
#include "attributes_getter_visitor.h"
#include "utils/macros.h"

using namespace ai;

AttributesGetterVisitor::AttributesGetterVisitor(std::vector<std::string> const &elementPath, BinaryXml::ElementAttributes &elementAttributes)
    : elementPath_(elementPath), elementAttributes_(elementAttributes) {}

auto AttributesGetterVisitor::visit(StartXmlTagElement const &element) -> void {
  currentElementPath_.push_back(element.tag());
  if (currentElementPath_ == elementPath_) {
    elementAttributes_ = element.attributes();
  }
}

auto AttributesGetterVisitor::visit(EndXmlTagElement const &element) -> void {
  utils::ignore(element);
  currentElementPath_.pop_back();
}

auto AttributesGetterVisitor::visit(InvalidXmlTagElement const &element) -> void { utils::ignore(element); }

auto AttributesGetterVisitor::visit(CDataTagElement const &element) -> void { utils::ignore(element); }
