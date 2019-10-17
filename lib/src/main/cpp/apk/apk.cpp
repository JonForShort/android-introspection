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
#include <codecvt>
#include <locale>
#include <string>
#include <vector>

#include "zip.h"
#include "apk/apk.h"
#include "utils/log.h"

using namespace ai;

namespace {

    auto constexpr XML_IDENTIFIER = 0x00080003;
    auto constexpr XML_STRING_TABLE = 0x0001;

    //
    // Resource Types
    //
    auto constexpr RES_TYPE_NULL = 0x00;
    auto constexpr RES_TYPE_REFERENCE = 0x01;
    auto constexpr RES_TYPE_ATTRIBUTE = 0x02;
    auto constexpr RES_TYPE_STRING = 0x03;
    auto constexpr RES_TYPE_FLOAT = 0x04;
    auto constexpr RES_TYPE_DIMENSION = 0x05;
    auto constexpr RES_TYPE_FRACTION = 0x06;
    auto constexpr RES_TYPE_DYNAMIC_REFERENCE = 0x07;
    auto constexpr RES_TYPE_INT_DEC = 0x10;
    auto constexpr RES_TYPE_INT_HEX = 0x11;
    auto constexpr RES_TYPE_INT_BOOLEAN = 0x12;

    //
    // Resource Values
    //
    auto constexpr RES_VALUE_TRUE = 0xffffffff;
    auto constexpr RES_VALUE_FALSE = 0x00000000;

    struct CompressedAndroidManifestHeader {
        uint32_t xmlMagicNumber;
        uint32_t reservedBytes;
        uint16_t stringTableIdentifier;
        uint16_t headerSize;
        uint32_t chunkSize;
        uint32_t numStrings;
        uint32_t numStyles;
        uint32_t flags;
        uint32_t stringsOffset;
        uint32_t stylesOffset;
    };

    struct UnzOpenFile {

        explicit UnzOpenFile(const char *szFileName) : zip_(unzOpen(szFileName)) {}

        ~UnzOpenFile() { unzClose(zip_); }

        auto get() const -> unzFile { return zip_; }

    private:
        unzFile zip_;
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

    template<typename T>
    auto readBytesFromVectorAtIndex(std::vector<uint8_t> const &data, size_t const index) {
        T value = {0};
        std::memcpy(&value, &data[index], sizeof(value));
        return value;
    }

    auto getStringsFromCompressedAndroidManifest(std::vector<uint8_t> const &contents) -> std::vector<std::string> {

        std::vector<std::string> strings;
        auto xmlHeader = reinterpret_cast<CompressedAndroidManifestHeader const *>(contents.data());
        if (xmlHeader->xmlMagicNumber != XML_IDENTIFIER) {
            LOGW("unable to get strings; compressed xml is invalid");
            return strings;
        }
        if (xmlHeader->stringTableIdentifier != XML_STRING_TABLE) {
            LOGW("unable to get strings; missing string marker");
            return strings;
        }

        auto stringOffsets = std::vector<uint32_t>();
        for (size_t i = 0; i < xmlHeader->numStrings; i++) {
            auto const index = sizeof(CompressedAndroidManifestHeader) + i * (sizeof(uint32_t));
            auto const offset = readBytesFromVectorAtIndex<uint32_t>(contents, index);
            stringOffsets.push_back(offset);
        }

        //
        // TODO: Figure out why we can't just use xmlheader->stringsOffset.  Using this will make us 8 bytes short of where strings really start.
        //

        auto const stringOffsetsInBytes = stringOffsets.size() * sizeof(decltype(stringOffsets)::value_type);
        auto const startStringsOffset = sizeof(CompressedAndroidManifestHeader) + stringOffsetsInBytes;
        auto const isUtf8Encoded = (xmlHeader->flags & 0x100U) > 0;
        for (auto &offset : stringOffsets) {
            if (isUtf8Encoded) {
                auto stringLength = readBytesFromVectorAtIndex<uint8_t>(contents, startStringsOffset + offset);
                auto stringOffset = reinterpret_cast<const char *>(contents.data() + startStringsOffset + offset + 2);
                auto string = std::string(stringOffset, stringLength);
                strings.push_back(string);
            } else {
                auto stringLength = readBytesFromVectorAtIndex<uint16_t>(contents, startStringsOffset + offset);
                auto stringOffset = reinterpret_cast<const char16_t *>(contents.data() + startStringsOffset + offset + 2);
                auto string = std::u16string(stringOffset, stringLength);
                std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
                strings.push_back(converter.to_bytes(string));
            }
        }

        return strings;
    }
}

auto apk::makeApkDebuggable(const char *apkPath) -> bool {
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
    auto const strings = getStringsFromCompressedAndroidManifest(contents);
    if (strings.empty()) {
        LOGW("unable to parse strings from AndroidManifest.xml in [%s]", apkPath);
        return false;
    }
    if (std::find(strings.begin(), strings.end(), "application") == strings.end()) {
        LOGW("unable to find application tag in AndroidManifest.xml in [%s]", apkPath);
        return false;
    }
    return true;
}
