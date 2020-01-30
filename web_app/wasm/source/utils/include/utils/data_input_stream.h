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
#ifndef ANDROID_INTROSPECTION_UTILS_DATA_INPUT_STREAM_H_
#define ANDROID_INTROSPECTION_UTILS_DATA_INPUT_STREAM_H_

#include <cstdint>
#include <vector>

class DataInputStream final {
public:
  DataInputStream(std::vector<std::byte> const &data) : data_(data) {}

  template <typename T> auto read() -> T {
    static_assert(std::is_integral<T>::value, "type must be integral");
    T value = {0};
    memcpy(&value, &data_[index_], sizeof(value));
    index_ += sizeof(value);
    return value;
  }

  auto reset() -> void;

  auto skip(uint32_t bytes) -> void;

private:
  std::size_t index_{0};

  std::vector<std::byte> data_;
};

#endif // ANDROID_INTROSPECTION_UTILS_DATA_INPUT_STREAM_H_
