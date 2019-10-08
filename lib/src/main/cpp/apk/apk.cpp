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
#include <algorithm>
#include <stdint.h>
#include <string>
#include <vector>

#include "zip.h"
#include "apk/apk.h"
#include "utils/log.h"

using namespace ai;

namespace {

    struct UnzOpenFile {

        UnzOpenFile(const char *szFileName) : zip_(unzOpen(szFileName)) {}

        ~UnzOpenFile() { unzClose(zip_); }

        auto get() const -> unzFile { return zip_; }

    private:
        unzFile const zip_;
    };

    auto getZipFileNames(const char *szFileName) -> std::vector<std::string> {
        std::vector<std::string> fileNames;
        auto const zip = UnzOpenFile(szFileName);
        if (zip.get() == nullptr) {
            LOGW("unable to open zip file [%s]", szFileName);
            return fileNames;
        }
        unz_global_info info;
        if (unzGetGlobalInfo(zip.get(), &info) != MZ_OK) {
            LOGW("unable to get global zip info [%s]", szFileName);
            return fileNames;
        }
        if (unzGoToFirstFile(zip.get()) != UNZ_OK) {
            LOGW("unable to get first zip file in archive [%s]", szFileName);
            return fileNames;
        }
        do {
            unz_file_info zipFileInfo;
            char szFilename[BUFSIZ] = {0};
            if (UNZ_OK == unzGetCurrentFileInfo(zip.get(), &zipFileInfo, szFilename, sizeof(szFilename), nullptr, 0, nullptr, 0)) {
                auto fileName = std::string(szFilename);
                fileNames.emplace_back(fileName);
            }
        } while (UNZ_OK == unzGoToNextFile(zip.get()));
        return fileNames;
    }

    auto getZipContents(const char *szFileName, const char *szZipFileName) -> std::vector<uint8_t> {
        auto contents = std::vector<uint8_t>();
        auto const zip = UnzOpenFile(szFileName);
        if (zip.get() == nullptr) {
            LOGW("unable to open zip file [%s]", szFileName);
            return contents;
        }
        if (unzLocateFile(zip.get(), szZipFileName, nullptr) != MZ_OK) {
            LOGW("unable to locate zip file [%s] [%s]", szFileName, szZipFileName);
            return contents;
        }
        unz_file_info zipFileInfo;
        if (unzGetCurrentFileInfo(zip.get(), &zipFileInfo, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK) {
            LOGW("unable to get current file info [%s] [%s]", szFileName, szZipFileName);
            return contents;
        }
        if (unzOpenCurrentFile(zip.get()) != MZ_OK) {
            LOGW("unable to open current file [%s] [%s]", szFileName, szZipFileName);
            return contents;
        }
        contents.resize(zipFileInfo.uncompressed_size);
        unzReadCurrentFile(zip.get(), &contents[0], contents.size());
        return contents;
    }
}

auto apk::make_apk_debuggable(const char *apkPath) -> bool {
    auto const fileNames = getZipFileNames(apkPath);
    auto const containsAndroidManifest = std::find(fileNames.begin(), fileNames.end(), "AndroidManifest.xml") != fileNames.end();
    if (!containsAndroidManifest) {
        LOGD("unable to find AndroidManifest.xml in [%s]", apkPath);
        return false;
    }

    auto const contents = getZipContents(apkPath, "AndroidManifest.xml");
    if (contents.empty()) {
        LOGW("unable to read AndroidManifest.xml in [%s]", apkPath);
        return false;
    }
    return true;
}