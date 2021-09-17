#include <esp_littlefs.h>
#include <esp_log.h>
#include <esp_spi_flash.h>
#include <esp_spiffs.h>
#include <esp_system.h>
#include <esp_vfs.h>
#include <sdkconfig.h>

#ifdef __cplusplus
extern "C" void app_main(void);
#endif
void init_lfs();
void init_spiffs();

static const char* TAG = "main";

void app_main(void) {
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
}

void init_lfs() {
  //
}

void init_spiffs() {
  //
}
