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

#ifdef WASM

#include <emscripten/bind.h>
#include <string>
#include <vector>

#include "apk/apk.h"
#include "utils/log.h"

using namespace emscripten;

auto initialize() { LOGV("wasm::initialize"); }

auto uninitialize() { LOGV("wasm::uninitialize"); }

auto isValid(std::string const pathToApk) {
  LOGV("wasm::isValid");
  auto const apk = ai::Apk(pathToApk);
  return apk.isValid();
}

auto getFiles(std::string const apkPath) {
  LOGV("wasm::getFiles apkPath [%s]", apkPath);
  auto const apk = ai::Apk(apkPath);
  return apk.getFiles();
}

EMSCRIPTEN_BINDINGS(ApkModule) {

  function("initialize", &initialize);

  function("uninitialize", &uninitialize);

  function("isValid", &isValid);

  function("getFiles", &getFiles);

  register_vector<std::string>("vector<string>");
}

#else

auto main(int argc, char *argv[]) -> int {
  (void)argc;
  (void)argv;
}

#endif
