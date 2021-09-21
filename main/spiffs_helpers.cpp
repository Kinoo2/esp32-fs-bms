#include "spiffs_helpers.h"

#include <esp_log.h>
#include <esp_spiffs.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

using namespace std;

static const char* TAG = "spiffs_helpers";

SpiffsHelper::SpiffsHelper(const int maxFiles,
                           const string& basePath,
                           const string& partitionLabel,
                           const bool formatIfMountFailed)
  : FsHelperBase(basePath, partitionLabel, formatIfMountFailed),
    _maxFiles{maxFiles} {
  ESP_LOGI(TAG, "Initializing SPIFFS");

  auto heap_start = esp_get_free_heap_size();
  auto start      = xTaskGetTickCount();

  esp_vfs_spiffs_conf_t cfg;
  cfg.base_path              = _basePath.c_str();
  cfg.partition_label        = _partitionLabel.c_str();
  cfg.format_if_mount_failed = _formatIfMountFailed;
  cfg.max_files              = _maxFiles;
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
