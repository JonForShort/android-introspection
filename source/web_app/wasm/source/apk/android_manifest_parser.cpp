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
#include "android_manifest_parser.h"

using namespace ai;

auto AndroidManifestParser::isValid() const -> bool { return binaryXml_.hasElement("application"); }

auto AndroidManifestParser::toStringXml() const -> std::string { return binaryXml_.toStringXml(); }

auto AndroidManifestParser::isApplicationDebuggable() const -> bool {
  auto const elementAttributes = binaryXml_.getElementAttributes(std::vector<std::string>{"manifest", "application"});
  auto const debuggableAttribute = elementAttributes.find("debuggable");
  if (debuggableAttribute != elementAttributes.end()) {
    return debuggableAttribute->second == "true";
  } else {
    return false;
  }
}

auto AndroidManifestParser::setApplicationDebuggable(bool const debuggable) const -> void {
  binaryXml_.setElementAttribute(std::vector<std::string>{"manifest", "application"}, "debuggable", debuggable ? "true" : "false");
}

auto AndroidManifestParser::getPackageName() const -> std::string {
  auto const elementAttributes = binaryXml_.getElementAttributes(std::vector<std::string>{"manifest"});
  auto const packageAttribute = elementAttributes.find("package");
  if (packageAttribute != elementAttributes.end()) {
    return packageAttribute->second;
  } else {
    return std::string();
  }
}

auto AndroidManifestParser::getVersionName() const -> std::string {
  auto const elementAttributes = binaryXml_.getElementAttributes(std::vector<std::string>{"manifest"});
  auto const versionNameAttribute = elementAttributes.find("versionName");
  if (versionNameAttribute != elementAttributes.end()) {
    return versionNameAttribute->second;
  } else {
    return std::string();
  }
}

auto AndroidManifestParser::getVersionCode() const -> std::string {
  auto const elementAttributes = binaryXml_.getElementAttributes(std::vector<std::string>{"manifest"});
  auto const versionCodeAttribute = elementAttributes.find("versionCode");
  if (versionCodeAttribute != elementAttributes.end()) {
    return versionCodeAttribute->second;
  } else {
    return std::string();
  }
}