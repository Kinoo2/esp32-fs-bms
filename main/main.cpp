#include "lfs_helpers.h"
#include "spiffs_helpers.h"

#include <string>
#include <esp_err.h>
#include <esp_heap_trace.h>
#include <esp_log.h>
#include <esp_spi_flash.h>
#include <esp_system.h>
#include <esp_vfs.h>
#include <sdkconfig.h>

#ifdef __cplusplus
extern "C" void app_main(void);
#endif

static const char* TAG = "main";

#define TRACE_RECORD_COUNT 300
static heap_trace_record_t traceRecord[TRACE_RECORD_COUNT];

void app_main(void) {
  heap_trace_init_standalone(traceRecord, TRACE_RECORD_COUNT);

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
  SpiffsHelper spiffs{100};

  int writeSize = 100000;
  for (int i = 0; i < 10; i++) {
    printf(
      "====================================================================="
      "===========\n");
    lfs.WriteText(writeSize);
    spiffs.WriteText(writeSize);
  }

  ESP_LOGI(TAG, "Free Heap: %d", esp_get_free_heap_size());
  ESP_LOGI(TAG, "sizeof(LfsHelper): %d", sizeof(LfsHelper));
  ESP_LOGI(TAG, "sizeof(SpiffsHelper): %d", sizeof(SpiffsHelper));
}
