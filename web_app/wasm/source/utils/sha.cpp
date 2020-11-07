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
#include <botan/hash.h>
#include <botan/hex.h>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "utils/log.h"
#include "utils/macros.h"
#include "utils/sha.h"

using namespace ai;

namespace {

static constexpr size_t FILE_READ_BUFFER_SIZE = 16 * 1024;

} // namespace

auto utils::sha::generateSha256ForFile(std::string const &path) -> std::string {
  std::ifstream file(path, std::ios::in | std::ios::binary);
  if (!file.good()) {
    LOGW("failed to open input file [{}]", path);
    return std::string();
  }

  std::vector<uint8_t> buffer(FILE_READ_BUFFER_SIZE);
  std::unique_ptr<Botan::HashFunction> hash(Botan::HashFunction::create("SHA-256"));
  while (file.good()) {
    file.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
    hash->update(buffer.data(), file.gcount());
  }

  return Botan::hex_encode(hash->final());
}