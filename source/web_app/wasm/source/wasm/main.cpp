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

#include <map>
#include <string>
#include <vector>

#include "apk/apk.h"
#include "utils/emscripten_bind_wrapper.h"
#include "utils/log.h"

using namespace emscripten;

auto initialize() { LOGV("wasm::initialize"); }

auto uninitialize() { LOGV("wasm::uninitialize"); }

namespace apk {

auto isValid(std::string const pathToApk) {
  LOGV("wasm::apk::isValid pathToApk [{}]", pathToApk);
  auto const apk = ai::Apk(pathToApk);
  return apk.isValid();
}

auto getFiles(std::string const pathToApk) {
  LOGV("wasm::apk::getFiles pathToApk [{}]", pathToApk);
  auto const apk = ai::Apk(pathToApk);
  return apk.getFiles();
}

auto getFileContent(std::string const pathToApk, std::string const pathToFile) {
  LOGV("wasm::apk::getFileContent pathToApk [{}] pathToFile [{}]", pathToApk, pathToFile);
  auto const apk = ai::Apk(pathToApk);
  auto const fileContent = apk.getFileContent(pathToFile);
}

auto getProperties(std::string const pathToApk) {
  LOGV("wasm::apk::getProperties pathToApk [{}]", pathToApk);
  auto const apk = ai::Apk(pathToApk);
  return apk.getProperties();
}

} // namespace apk

EMSCRIPTEN_BINDINGS(ApkModule) {

  function("initialize", &initialize);

  function("uninitialize", &uninitialize);

  function("isValid", &apk::isValid);

  function("getFiles", &apk::getFiles);

  function("getFileContent", &apk::getFileContent);

  function("getProperties", &apk::getProperties);

  register_vector<std::string>("vector<string>");

  register_map<std::string, std::string>("map<string, string>");
}

#else

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <iostream>

#include "apk/apk.h"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

auto main(int argc, char *argv[]) -> int {

  std::string file_argument;
  bool print_manifest = false;

  try {
    po::options_description desc{"Options"};
    desc.add_options()("help,h", "Help screen")("manifest,pm", po::bool_switch(&print_manifest),
                                                "print manifest")("file,f", po::value<std::string>(&file_argument)->required(), "file path to apk");

    po::variables_map vm;
    po::store(parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  } catch (const po::error &ex) {
    std::cerr << ex.what() << '\n';
    return -1;
  }

  const fs::path file_path(file_argument);
  if (!fs::exists(file_path)) {
    std::cerr << "file path does not exist; please check path" << std::endl;
    return -2;
  }

  if (fs::is_directory(file_path)) {
    std::cerr << "file path is a directory; please check path" << std::endl;
    return -3;
  }

  if (print_manifest) {
    auto const apk = ai::Apk(file_argument);
    std::cout << apk.getAndroidManifest() << std::endl;
  }
}

#endif
