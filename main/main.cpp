#include <esp_err.h>
#include <esp_littlefs.h>
#include <esp_log.h>
#include <esp_spi_flash.h>
#include <esp_spiffs.h>
#include <esp_system.h>
#include <esp_vfs.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
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

  init_lfs();
  init_spiffs();
}

void init_lfs() {
  ESP_LOGI(TAG, "Initializing LittleFS");

  auto heap_start = esp_get_free_heap_size();
  auto start      = xTaskGetTickCount();

  esp_vfs_littlefs_conf_t cfg;
  cfg.base_path              = "/lfs";
  cfg.partition_label        = "littlefs";
  cfg.format_if_mount_failed = true;
  cfg.dont_mount             = false;
  auto ret                   = esp_vfs_littlefs_register(&cfg);

  auto end      = xTaskGetTickCount();
  auto heap_end = esp_get_free_heap_size();

  if (ret == ESP_FAIL) {
    ESP_LOGE(TAG, "Failed to mount or format LittleFS filesystem");
    abort();
  }
  else if (ret == ESP_ERR_NOT_FOUND) {
    ESP_LOGE(TAG, "Failed to find LittleFS partition");
    abort();
  }
  else if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
    abort();
  }

  ESP_LOGI(TAG, "Initialized LittleFS in %d ms", (end - start) * 10);
  ESP_LOGI(TAG, "Net bytes allocated: %d", heap_start - heap_end);

  size_t total = 0, used = 0;
  ret = esp_littlefs_info(cfg.partition_label, &total, &used);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG,
             "Failed to get LittleFS partition information (%s)",
             esp_err_to_name(ret));
  }
  else {
    ESP_LOGI(TAG, "LittleFS partition size| total: %d, used: %d", total, used);
  }
}

void init_spiffs() {
  ESP_LOGI(TAG, "Initializing SPIFFS");

  auto heap_start = esp_get_free_heap_size();
  auto start      = xTaskGetTickCount();

  esp_vfs_spiffs_conf_t cfg;
  cfg.base_path              = "/spiffs";
  cfg.partition_label        = "spiffs";
  cfg.format_if_mount_failed = true;
  cfg.max_files              = 50;
  auto ret                   = esp_vfs_spiffs_register(&cfg);

  auto end      = xTaskGetTickCount();
  auto heap_end = esp_get_free_heap_size();

  if (ret == ESP_FAIL) {
    ESP_LOGE(TAG, "Failed to mount or format SPIFFS filesystem");
    abort();
  }
  else if (ret == ESP_ERR_NOT_FOUND) {
    ESP_LOGE(TAG, "Failed to find SPIFFS partition");
    abort();
  }
  else if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    abort();
  }

  ESP_LOGI(TAG, "Initialized SPIFFS in %d ms", (end - start) * 10);
  ESP_LOGI(TAG, "Net bytes allocated: %d", heap_start - heap_end);

  size_t total = 0, used = 0;
  ret = esp_spiffs_info(cfg.partition_label, &total, &used);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG,
             "Failed to get SPIFFS partition information (%s)",
             esp_err_to_name(ret));
  }
  else {
    ESP_LOGI(TAG, "SPIFFS partition size| total: %d, used: %d", total, used);
  }
}
