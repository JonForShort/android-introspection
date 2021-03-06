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
#ifndef ANDROID_INTROSPECTION_UTILS_UTILS_H_
#define ANDROID_INTROSPECTION_UTILS_UTILS_H_

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

using namespace std;

namespace ai::utils {

inline auto fileExists(std::string_view path) -> bool {
  ifstream infile(path.data());
  return infile.good();
}

inline auto writeToFile(std::string const &path, std::string const &contents) {
  ofstream outfile(path.data());
  outfile << contents;
}

inline auto bytesToHexString(std::vector<std::byte> const &bytes) -> std::string {
  std::ostringstream stream;
  stream << std::hex << std::uppercase << std::setfill('0');
  for (auto const b : bytes) {
    stream << std::setw(2) << static_cast<char>(b);
  }
  return stream.str();
}

template <typename... Args> std::string formatString(const char *format, Args... args) {
  auto size = static_cast<uint64_t>(snprintf(nullptr, 0, format, args...) + 1);
  std::unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format, args...);
  return std::string(buf.get(), buf.get() + size - 1);
}

} // namespace ai::utils

#endif /* ANDROID_INTROSPECTION_UTILS_UTILS_H_ */
