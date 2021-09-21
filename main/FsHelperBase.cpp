#include "FsHelperBase.h"

#include <cstring>
#include <esp_log.h>
#include <esp_vfs.h>

using namespace std;

static const char* TAG = "FsHelperBase";

// https://www.lipsum.com/
// Generated 5 paragraphs, 358 words, 2449 bytes of Lorem Ipsum
static const char* lipsum =
  "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum leo "
  "ipsum, dictum et porttitor eget, lacinia non lorem. Pellentesque convallis "
  "a risus non vehicula. Mauris efficitur euismod magna, at blandit urna "
  "malesuada sed. Etiam leo ex, consectetur non velit id, pharetra malesuada "
  "felis. Curabitur a tincidunt velit. Quisque imperdiet mollis pulvinar. Sed "
  "scelerisque erat vel laoreet scelerisque. Praesent imperdiet purus feugiat "
  "mauris lacinia mollis. Cras sed urna ac turpis viverra auctor sit amet "
  "maximus metus."
  "Nam vitae nisi eros. Vivamus pharetra non ante ullamcorper tempor. Vivamus "
  "commodo justo ac varius lobortis. Mauris luctus urna et dui aliquam "
  "laoreet. Donec nec urna eu nisl porttitor hendrerit et vitae ante. Integer "
  "tristique at leo ac rutrum. Pellentesque malesuada fringilla purus non "
  "volutpat."
  "Donec cursus, leo et vestibulum luctus, elit risus mattis nunc, vitae "
  "lobortis metus lacus ultrices leo. Etiam nec eleifend ipsum. Fusce "
  "tristique fermentum augue suscipit dapibus. Maecenas sed magna id quam "
  "rutrum viverra dapibus in ipsum. Donec ac lacus auctor, elementum nulla in, "
  "feugiat purus. Vivamus imperdiet ullamcorper lectus vitae lacinia. "
  "Phasellus sollicitudin tincidunt sapien at pharetra."
  "Curabitur tempor aliquet lectus, ac lobortis ex suscipit a. Quisque "
  "lobortis magna non felis hendrerit lacinia. Nunc mollis, mi sit amet tempus "
  "mollis, augue purus consequat massa, at viverra lacus libero semper turpis. "
  "Quisque pretium efficitur nisi in bibendum. Nam sodales malesuada eros "
  "varius ornare. In eleifend est eros, posuere euismod justo vehicula vitae. "
  "Integer eget ligula mi. Fusce accumsan pulvinar tortor in sagittis. In "
  "porta, est vitae posuere porttitor, lectus odio mollis felis, laoreet "
  "dapibus odio turpis eu eros. Praesent at velit efficitur, tincidunt mauris "
  "non, viverra eros. Suspendisse sit amet elit sollicitudin lacus ultricies "
  "lacinia. Quisque rutrum quam metus, id aliquet arcu elementum ac. Fusce "
  "pulvinar pharetra elit eget sagittis. Mauris consectetur quis risus eu "
  "interdum. Donec mauris mi, pellentesque vel varius eu, viverra et lacus. In "
  "euismod nulla in dolor ultrices lacinia."
  "In at leo hendrerit, dictum lectus eu, rhoncus leo. Mauris at mi vitae "
  "dolor consectetur bibendum vitae vel nulla. Praesent lobortis mi eget neque "
  "molestie commodo. Nulla vehicula dapibus egestas. Quisque viverra enim "
  "gravida fermentum hendrerit. Sed eu eleifend velit. Aenean mollis egestas "
  "ex ut eleifend.";

void FsHelperBase::WriteText(uint32_t numBytes) {
  if (numBytes > strlen(lipsum)) {
    ESP_LOGE(TAG, "Too Long. Sample text is only %d bytes.", strlen(lipsum));
  }
  auto name = getNextName();

  FILE* f = fopen(name.c_str(), "w");
}

string FsHelperBase::getNextName() {
  uint32_t width = 10;
  char filenameStr[width + 1];
  snprintf(filenameStr, width + 1, "%0*d", width, _fileIndex);
  _fileIndex++;
  return _basePath + string{filenameStr};
}
