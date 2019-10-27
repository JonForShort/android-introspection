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
#ifndef ANDROID_INTROSPECTION_APK_RESOURCE_TYPES_H_
#define ANDROID_INTROSPECTION_APK_RESOURCE_TYPES_H_

#include <cstdint>

namespace ai {

enum : uint32_t {

  XML_IDENTIFIER = 0x00080003,
  XML_STRING_TABLE = 0x0001,
  XML_ATTRS_MARKER = 0x00140014,
};

//
// Control Types
//
enum : uint16_t {

  RES_NULL_TYPE = 0x0000,
  RES_STRING_POOL_TYPE = 0x0001,
  RES_TABLE_TYPE = 0x0002,

  RES_XML_TYPE = 0x0003,
  RES_XML_FIRST_CHUNK_TYPE = 0x0100,
  RES_XML_START_NAMESPACE_TYPE = 0x0100,
  RES_XML_END_NAMESPACE_TYPE = 0x0101,
  RES_XML_START_ELEMENT_TYPE = 0x0102,
  RES_XML_END_ELEMENT_TYPE = 0x0103,
  RES_XML_CDATA_TYPE = 0x0104,
  RES_XML_LAST_CHUNK_TYPE = 0x017f,
  RES_XML_RESOURCE_MAP_TYPE = 0x0180,

  RES_TABLE_PACKAGE_TYPE = 0x0200,
  RES_TABLE_TYPE_TYPE = 0x0201,
  RES_TABLE_TYPE_SPEC_TYPE = 0x0202,
  RES_TABLE_LIBRARY_TYPE = 0x0203,
  RES_TABLE_OVERLAYABLE_TYPE = 0x0204,
  RES_TABLE_OVERLAYABLE_POLICY_TYPE = 0x0205,
};

//
// Resource Types
//
enum : uint8_t {

  TYPE_NULL = 0x00,
  TYPE_REFERENCE = 0x01,
  TYPE_ATTRIBUTE = 0x02,
  TYPE_STRING = 0x03,
  TYPE_FLOAT = 0x04,
  TYPE_DIMENSION = 0x05,
  TYPE_FRACTION = 0x06,
  TYPE_DYNAMIC_REFERENCE = 0x07,
  TYPE_DYNAMIC_ATTRIBUTE = 0x08,
  TYPE_FIRST_INT = 0x10,
  TYPE_INT_DEC = 0x10,
  TYPE_INT_HEX = 0x11,
  TYPE_INT_BOOLEAN = 0x12,
  TYPE_FIRST_COLOR_INT = 0x1c,
  TYPE_INT_COLOR_ARGB8 = 0x1c,
  TYPE_INT_COLOR_RGB8 = 0x1d,
  TYPE_INT_COLOR_ARGB4 = 0x1e,
  TYPE_INT_COLOR_RGB4 = 0x1f,
  TYPE_LAST_COLOR_INT = 0x1f,
  TYPE_LAST_INT = 0x1f
};

//
// Resource Values
//
enum : uint32_t {

  RES_VALUE_TRUE = 0xffffffff,
  RES_VALUE_FALSE = 0x00000000
};

//
// Resource Flags
//
auto constexpr RES_FLAG_UTF8 = 1 << 8;

} // namespace ai

#endif /* ANDROID_INTROSPECTION_APK_RESOURCE_TYPES_H_ */
