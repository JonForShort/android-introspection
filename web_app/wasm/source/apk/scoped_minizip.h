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
#ifndef ANDROID_INTROSPECTION_APK_SCOPED_MINIZIP_H_
#define ANDROID_INTROSPECTION_APK_SCOPED_MINIZIP_H_

#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>
#include <zip.h>

namespace ai::minizip {

struct ScopedZipOpen {

  explicit ScopedZipOpen(char const *const szFileName, int const mode) : zip_(zipOpen(szFileName, mode)) {}

  ~ScopedZipOpen() { zipClose(zip_, nullptr); }

  auto get() const -> zipFile { return zip_; }

private:
  zipFile const zip_;
};

struct ScopedUnzOpenFile {

  explicit ScopedUnzOpenFile(char const *const szFileName) : zip_(unzOpen(szFileName)) {}

  ~ScopedUnzOpenFile() { unzClose(zip_); }

  auto get() const -> unzFile { return zip_; }

private:
  unzFile zip_;
};

struct ScopedUnzOpenCurrentFile {

  explicit ScopedUnzOpenCurrentFile(unzFile const zipFile) : zipFile_(zipFile), result_(unzOpenCurrentFile(zipFile)) {}

  ~ScopedUnzOpenCurrentFile() {
    if (result_ == MZ_OK) {
      unzCloseCurrentFile(zipFile_);
    }
  }

  auto result() const -> int { return result_; }

private:
  unzFile const zipFile_;

  int const result_;
};

struct ScopedMzZipWriterDelete {

  explicit ScopedMzZipWriterDelete(void *handle) : handle_(handle) {}

  ~ScopedMzZipWriterDelete() { mz_zip_writer_delete(&handle_); }

private:
  void *handle_;
};

struct ScopedMzZipEntryClose {

  explicit ScopedMzZipEntryClose(void *handle) : handle_(handle) {}

  ~ScopedMzZipEntryClose() { mz_zip_entry_close(handle_); }

private:
  void *handle_;
};

struct ScopedMzStreamClose {

  explicit ScopedMzStreamClose(void *handle) : handle_(handle) {}

  ~ScopedMzStreamClose() { mz_stream_close(handle_); }

private:
  void *handle_;
};

} // namespace ai::minizip

#endif /* ANDROID_INTROSPECTION_APK_SCOPED_MINIZIP_H_ */
