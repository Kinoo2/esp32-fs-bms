#include "lfs_helpers.h"

#include <esp_littlefs.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char* TAG = "lfs_helpers";

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
