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
#include "binary_xml_element.h"
#include "binary_xml_visitor.h"

using namespace ai;

BinaryXmlElement::~BinaryXmlElement() = default;

auto StartXmlTagElement::tag() const -> std::string { return tag_; }

auto StartXmlTagElement::nameSpace() const -> std::string { return nameSpace_; }

auto StartXmlTagElement::attributes() const -> std::map<std::string, std::string> { return attributes_; }

auto StartXmlTagElement::accept(BinaryXmlVisitor &visitor) const -> void { visitor.visit(*this); }

StartXmlTagElement::~StartXmlTagElement() = default;

auto EndXmlTagElement::tag() const -> std::string { return tag_; }

auto EndXmlTagElement::nameSpace() const -> std::string { return nameSpace_; }

auto EndXmlTagElement::accept(BinaryXmlVisitor &visitor) const -> void { visitor.visit(*this); }

EndXmlTagElement::~EndXmlTagElement() = default;

auto CDataTagElement::tag() const -> std::string { return tag_; }

auto CDataTagElement::accept(BinaryXmlVisitor &visitor) const -> void { visitor.visit(*this); }

CDataTagElement::~CDataTagElement() = default;

auto InvalidXmlTagElement::tag() const -> std::string { return ""; }

auto InvalidXmlTagElement::accept(BinaryXmlVisitor &visitor) const -> void { visitor.visit(*this); }

auto InvalidXmlTagElement::error() const -> std::string { return error_; }

InvalidXmlTagElement::~InvalidXmlTagElement() = default;
