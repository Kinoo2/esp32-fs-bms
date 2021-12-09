#include "lipsum.h" // This just contains the char* lipsum, which is a bunch of lorem ipsum.

#include <fstream>
#include <esp_littlefs.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifdef __cplusplus
extern "C" void app_main(void);
#endif

void app_main(void) {
  esp_vfs_littlefs_conf_t cfg;
  cfg.base_path              = "/lfs";
  cfg.partition_label        = "littlefs";
  cfg.format_if_mount_failed = true;
  cfg.dont_mount             = false;
  esp_vfs_littlefs_register(&cfg);

  std::fstream strm;
  strm.open("/lfs/lips.txt",
            std::fstream::in | std::fstream::out | std::fstream::app);

  printf("tellp:%d tellg:%d\n", (int)strm.tellp(), (int)strm.tellg());

  // Read.
  strm.seekg(0);
  char* data = (char*)malloc(10000);
  auto len   = strm.readsome(data, 10000);
  printf("Reading %d bytes.\n", len);

  data[len] = '\0';
  printf("%s\n", data);

  // Write.
  strm.write(lipsum, 2048);
  strm.flush();

  // strm.close();

  // Delay forever so the stream doesn't go out of scope and close
  // automatically.
  vTaskDelay(portMAX_DELAY);
}
