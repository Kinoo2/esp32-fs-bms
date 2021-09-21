#include "FsHelperBase.h"

#include <esp_log.h>

using namespace std;

static const char* TAG = "FsHelperBase";

bool FsHelperBase::Write(uint32_t numBytes) {
  auto name = getNextName();

  return true;
}

string FsHelperBase::getNextName() {
  uint32_t width = 10;
  char filenameStr[width + 1];
  snprintf(filenameStr, width + 1, "%0*d", width, _fileIndex);
  _fileIndex++;
  return _basePath + string{filenameStr};
}
