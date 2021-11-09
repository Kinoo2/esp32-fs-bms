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
  fclose(f);

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
