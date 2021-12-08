#include "lfs_helpers.h"
#include "lipsum.h"
#include "spiffs_helpers.h"

#include <locale>
#include <string>
#include <esp_err.h>
#include <esp_heap_trace.h>
#include <esp_log.h>
#include <esp_spi_flash.h>
#include <esp_system.h>
#include <esp_vfs.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <sdkconfig.h>

#ifdef __cplusplus
extern "C" void app_main(void);
#endif

static const char* TAG = "main";

#define TRACE_RECORD_COUNT 300
static heap_trace_record_t traceRecord[TRACE_RECORD_COUNT];

static const int writeSize  = 100000;
static const int iterations = 15;
static const int maxFiles   = 60;

void app_main(void) {
  setlocale(LC_NUMERIC, "");

  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  ESP_LOGI(TAG,
           "System Info:\n"
           "\t\tchip: %s\n"
           "\t\tnum cores: %d\n"
           "\t\tWifi%s%s\n"
           "\t\tSi rev: %d\n"
           "\t\tFlash size: %dMB %s\n"
           "\t\tFree heap: %d\n",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "",
           chip_info.revision,
           spi_flash_get_chip_size() / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded"
                                                         : "external",
           esp_get_free_heap_size());

  LfsHelper lfs;
  SpiffsHelper spiffs{maxFiles};

  int fileId = 1;

  lfs.openFile(fileId);
  spiffs.openFile(fileId);

  printf("\e[0;34m====Read: %03d====\n\e[0m", 1);
  lfs.readFromFile();
  spiffs.readFromFile();

  lfs.writeToFile(lipsum, 2048);
  spiffs.writeToFile(lipsum, 2048);

  vTaskDelay(portMAX_DELAY);
}
