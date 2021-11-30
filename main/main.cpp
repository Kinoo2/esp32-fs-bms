#include "lfs_helpers.h"
#include "spiffs_helpers.h"

#include <locale>
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

static const int writeSize  = 100000;
static const int iterations = 15;
static const int maxFiles   = 60;

void app_main(void) {
  heap_trace_init_standalone(traceRecord, TRACE_RECORD_COUNT);

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

  int fileId = 3;

  /** WRITE (fprintf) **/
  lfs.openFile(fileId);
  spiffs.openFile(fileId);
  for (int i = 0; i < iterations; i++) {
    printf("\e[0;34m====Write: %03d====\n\e[0m", i);
    auto txt = std::to_string(i) + "\n";
    lfs.writeToFile(txt.c_str(), txt.size());
    spiffs.writeToFile(txt.c_str(), txt.size());
  }

  // /** READ (getc) **/
  // for (int i = 0; i < 1; i++) {
  printf("\e[0;34m====Read: %03d====\n\e[0m", 1);
  lfs.ReadText(fileId);
  spiffs.ReadText(fileId);
  // }

  /** DELETE (remove) **/
  // for (int i = 0; i < iterations; i++) {
  //   printf("\e[0;34m====Delete: %03d====\n\e[0m", i);
  //   lfs.DeleteFile(i);
  //   spiffs.DeleteFile(i);
  // }

  ESP_LOGI(TAG, "Free Heap: %d", esp_get_free_heap_size());
  ESP_LOGI(TAG,
           "Largest Block: %d",
           heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  ESP_LOGI(TAG,
           "Minimum Heap Size: %d",
           heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT));
}
