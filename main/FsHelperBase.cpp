#include "FsHelperBase.h"

#include "lipsum.h"

#include <cstring>
#include <errno.h>
#include <esp_heap_trace.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_vfs.h>

using namespace std;

static const char* TAG = "FsHelperBase";

int64_t FsHelperBase::WriteText(uint32_t numBytes) {
  if (numBytes > strlen(lipsum)) {
    ESP_LOGE(TAG, "Too Long. Sample text is only %d bytes.", strlen(lipsum));
    return -1;
  }
  auto name = getNextName();

  heap_trace_start(HEAP_TRACE_ALL);
  auto start = esp_timer_get_time();
  FILE* f    = fopen(name.c_str(), "w");
  if (f == nullptr) {
    ESP_LOGE(TAG,
             "Failed to open file [%s] for writing: (%d): %s",
             name.c_str(),
             errno,
             strerror(errno));
    return -1;
  }

  auto numWritten = fprintf(f, "%*.*s", numBytes, numBytes, lipsum);
  fclose(f);
  auto end = esp_timer_get_time();
  heap_trace_stop();
  // heap_trace_dump();

  ESP_LOGI(TAG,
           "Wrote %d/%d bytes in %lld μs to %s",
           numWritten,
           numBytes,
           end - start,
           name.c_str());

  return end - start;
}

int64_t FsHelperBase::ReadText(int fileId) {
  int fsize = 0;
  auto path = getPath(fileId);

  // heap_trace_start(HEAP_TRACE_ALL);
  auto start = esp_timer_get_time();
  FILE* f    = fopen(path.c_str(), "r");
  if (f == nullptr) {
    ESP_LOGE(TAG,
             "Failed to open file [%s] for reading. (%d): %s",
             path.c_str(),
             errno,
             strerror(errno));
    return -1;
  }

  int x = fgetc(f);
  int i = 0;
  while ((x != EOF) && (i < 100)) {
    putc((char)x, stdout);
    fsize++;
    i++;
    x = fgetc(f);
  }
  // fclose(f);

  auto end = esp_timer_get_time();
  putc('\n', stdout);
  // heap_trace_stop();
  // heap_trace_dump();

  ESP_LOGI(
    TAG, "Read %d bytes in %lld μs from %s", fsize, end - start, path.c_str());

  return end - start;
}

void FsHelperBase::DeleteFile(int fileId) {
  auto path = getPath(fileId);

  // heap_trace_start(HEAP_TRACE_ALL);
  auto start = esp_timer_get_time();
  remove(path.c_str());
  auto end = esp_timer_get_time();
  // heap_trace_stop();
  // heap_trace_dump();

  ESP_LOGI(TAG, "Deleted [%s] in %lld μs", path.c_str(), end - start);
}

string FsHelperBase::getNextName() { return getPath(_fileIndex++); }

string FsHelperBase::getPath(int fileId) {
  uint32_t width = 4;
  char filenameStr[width + 1];
  snprintf(filenameStr, width + 1, "%0*d", width, fileId);
  return _basePath + "/" + string{filenameStr};
}

void FsHelperBase::openFile(uint32_t fileIndex) {
  auto path = getPath(fileIndex);
  struct stat st;
  stat(path.c_str(), &st);
  printf("stat len=%d\n", (int)st.st_size);

  if (_currFileStream == nullptr) {
    _currFileStream = make_shared<fstream>();
  }

  if (!_currFileStream->is_open()) {
    // Open file in "append" mode.
    _currFileStream->open(path.c_str(),
                          fstream::in | fstream::out | fstream::app);
    auto st  = _currFileStream->rdstate();
    auto len = _currFileStream->tellp();
    printf("_currFileStream->rdstate()=%d, len=%d\n", st, (int)len);

    _currFileStream->seekp(0, fstream::end);
    st  = _currFileStream->rdstate();
    len = _currFileStream->tellp();
    printf("_currFileStream->rdstate()=%d, len=%d\n", st, (int)len);

    _fileIndex = fileIndex;
    printf("Opening file: %s\n", path.c_str());
  }
}

void FsHelperBase::closeFile() {
  printf("Closing file: %s\n", getPath(_fileIndex).c_str());
  if (_currFileStream != nullptr && _currFileStream->is_open()) {
    _currFileStream->flush();
    _currFileStream->close();
  }
  _currFileStream.reset();
}

void FsHelperBase::writeToFile(const char* data, size_t size) {
  if (_currFileStream == nullptr || !_currFileStream->is_open()) {
    printf("ERROR: No file open!");
    return;
  }

  printf("Writing %d bytes to %s.\n", size, getPath(_fileIndex).c_str());
  _currFileStream->write(data, size);
  _currFileStream->flush();
}

void FsHelperBase::readFromFile() {
  if (_currFileStream == nullptr || !_currFileStream->is_open()) {
    printf("ERROR: No file open!");
    return;
  }

  _currFileStream->seekp(0);

  char* data = (char*)malloc(10000);
  auto len   = _currFileStream->readsome(data, 10000);
  printf("Reading %d bytes from %s.\n", len, getPath(_fileIndex).c_str());

  data[len] = '\0';
  printf("%s\n", data);
}
