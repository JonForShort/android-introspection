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
#include <string>
#include <vector>

#include "zip.h"
#include "apk/apk.h"

using namespace ai;

namespace {

    auto getZipFileNames(const char *szZipArchive) -> std::vector<std::string> {
        std::vector<std::string> fileNames;
        auto const zip = unzOpen(szZipArchive);
        if (zip) {
            unz_global_info info;
            if (MZ_OK == unzGetGlobalInfo(zip, &info)) {
                if (UNZ_OK == unzGoToFirstFile(zip)) {
                    do {
                        char szFilename[BUFSIZ];
                        if (UNZ_OK == unzGetCurrentFileInfo(zip, nullptr, szFilename, sizeof(szFilename), nullptr, 0, nullptr, 0)) {
                            fileNames.emplace_back(std::string(szFilename));
                        }
                    } while (UNZ_OK == unzGoToNextFile(zip));
                }
            }
        }
        return fileNames;
    }
}

auto apk::make_apk_debuggable(const char *apkPath) -> bool {
    auto const fileNames = getZipFileNames(apkPath);
    return true;
}