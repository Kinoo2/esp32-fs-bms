#include "FsHelperBase.h"

#include "lipsum.h"

#include <cstring>
#include <esp_heap_trace.h>
#include <esp_log.h>
#include <esp_vfs.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

using namespace std;

static const char* TAG = "FsHelperBase";

void FsHelperBase::WriteText(uint32_t numBytes) {
  if (numBytes > strlen(lipsum)) {
    ESP_LOGE(TAG, "Too Long. Sample text is only %d bytes.", strlen(lipsum));
    return;
  }
  auto name = getNextName();

  // heap_trace_start(HEAP_TRACE_ALL);
  auto start = xTaskGetTickCount();
  FILE* f    = fopen(name.c_str(), "w");
  if (f == nullptr) {
    ESP_LOGE(TAG, "Failed to open file [%s] for writing", name.c_str());
    return;
  }

  auto numWritten = fprintf(f, "%*.*s", numBytes, numBytes, lipsum);
  fclose(f);
  auto end = xTaskGetTickCount();
  // heap_trace_stop();
  // heap_trace_dump();

  ESP_LOGI(TAG,
           "Wrote %d/%d bytes in %dms to %s",
           numWritten,
           numBytes,
           pdTICKS_TO_MS(end - start),
           name.c_str());
}

string FsHelperBase::getNextName() {
  uint32_t width = 10;
  char filenameStr[width + 1];
  snprintf(filenameStr, width + 1, "%0*d", width, _fileIndex);
  _fileIndex++;
  return _basePath + "/" + string{filenameStr};
}
