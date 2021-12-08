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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#ifdef __cplusplus
extern "C" void app_main(void);
#endif

static const char* TAG = "main";

#define TRACE_RECORD_COUNT 300
static heap_trace_record_t traceRecord[TRACE_RECORD_COUNT];

static const int writeSize  = 100000;
static const int iterations = 15;
static const int maxFiles   = 60;

static const char block[] = "I (89963) NimBLEServer: subscribe event; attr_handle=93, subscribed: true\n"
"I (89963) NimBLECharacteristic: New subscribe value for conn: 0 val: 1\n"
"I (90023) KC-PlaybackManager: Play request: KCLFX_GenSuccConfirm_V1.wv\n"
"I (90023) KC-PlaybackManager: Now Playing: KCLFX_GenSuccConfirm_V1.wv (/spiffs/KCLFX_GenSuccConfirm_V1.wv)\n"
"I (90043) WAVPACK-PLAYER: WavPack file info: sample rate = 16000, channels = 1, bytes/sample = 2\n"
"I (90043) I2S: DMA Malloc info, datalen=blocksize=2048, dma_buf_count=8\n"
"I (90063) I2S: APLL: Req RATE: 16000, real rate: 15999.986, BITS: 16, CLKM: 1, BCK_M: 8, MCLK: 4095996.500, SCLK: 511999.562500, diva: 1, divb: 0\n"
"I (90083) KC-LightManager: Got name [KCLFX_GenSuccConfirm_V1.lit] from event\n"
"I (90083) KC-LightManager: Got name [KCLFX_GenSuccConfirm_V1.lit] from queue\n"
"I (90093) KC-LightManager: Got flatbuffer of size: 2044\n"
"W (90093) KC-KcTimer: Stop: Timer not running\n"
"I (90113) KC-LightManager: Verifying flatbuffer...\n"
"I (90113) KC-LightManager: Getting framerate and starting timer.\n"
"I (90223) NimBLEServer: subscribe event; attr_handle=122, subscribed: true\n"
"I (90233) NimBLECharacteristic: New subscribe value for conn: 0 val: 1\n"
"I (90233) KC-BLE WifiService: WifiStatusCbs Subscribing...\n"
"I (90323) KC-PlaybackManager: Finished playing sound\n"
"I (90343) NimBLEServer: subscribe event; attr_handle=105, subscribed: true\n"
"I (90353) NimBLECharacteristic: New subscribe value for conn: 0 val: 1\n"
"I (90473) KC-MqttManager: PUBLISHED, msg_id=50626\n"
"I (90643) KC-PlaybackManager: Play request: KCLFX_OnConnWiFi_V1.wv\n"
"I (90643) KC-PlaybackManager: Now Playing: KCLFX_OnConnWiFi_V1.wv (/spiffs/KCLFX_OnConnWiFi_V1.wv)\n"
"I (90653) WAVPACK-PLAYER: WavPack file decoded in 605 ms, samples expected / received = 11329 / 5632\n"
"I (90713) KC-LightManager: Got name [KCLFX_OnConnWiFi_V1.lit] from event\n"
"I (90713) KC-LightManager: Got name [KCLFX_OnConnWiFi_V1.lit] from queue while [KCLFX_GenSuccConfirm_V1.lit] is running\n"
"W (90713) KC-LightManager: Animation state is not 'Running'. Early exit from animation\n"
"I (90733) KC-LightManager: Got flatbuffer of size: 2324\n"
"W (90733) KC-KcTimer: Stop: Timer not running\n"
"I (90753) KC-LightManager: Verifying flatbuffer...\n"
"I (90753) KC-LightManager: Getting framerate and starting timer.\n"
"I (90913) KC-BLE WifiService: len: 9, ip:10.0.0.83\n"
"I (90933) WAVPACK-PLAYER: all files finished playing, audio torn down, 5632 samples, 0.0% compressed, 0.9% clipped\n"
"I (90953) WAVPACK-PLAYER: WavPack file info: sample rate = 16000, channels = 1, bytes/sample = 2\n"
"I (90953) I2S: DMA Malloc info, datalen=blocksize=2048, dma_buf_count=8\n"
"I (90963) I2S: APLL: Req RATE: 16000, real rate: 15999.986, BITS: 16, CLKM: 1, BCK_M: 8, MCLK: 4095996.500, SCLK: 511999.562500, diva: 1, divb: 0\n"
"I (91033) KC-UpdateService: New Update topic: [kc/app-0.03.00.60/update]\n"
"I (91043) KC-UpdateMqttService: UpdateMqttService::onSetUpdateTopic\n"
"I (91043) KC-UpdateMqttService: Changing update topic to [kc/app-0.03.00.60/update]\n"
"I (91053) KC-KcMqttClient: Unsubscribing [kc/KC-21LE-YZ11-LUX6/update]\n"
"I (91063) KC-KcMqttClient: Subscribing [kc/app-0.03.00.60/update], qos=0\n"
"I (91063) KC-UpdateService: UpdateTopicCbs::onSetValueEvent\n"
"I (91073) KC-MqttManager: MqttManager::onBackendUpdateEvent\n";

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

  int fileId = 6;

  printf("---------\nI am going to print:\n%s\n---------\n",block);

  /** WRITE (fprintf) **/
  lfs.openFile(fileId);

  lfs.readFromFile();

  lfs.writeToFile(block, sizeof(block));

  ESP_LOGI(TAG, "Free Heap: %d", esp_get_free_heap_size());
  ESP_LOGI(TAG,
           "Largest Block: %d",
           heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  ESP_LOGI(TAG,
           "Minimum Heap Size: %d",
           heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT));

  //without this closeFile, the read doesn't work on the next run
  lfs.closeFile();

  while(1) {
    vTaskDelay(pdMS_TO_TICKS(400));
  }
}
