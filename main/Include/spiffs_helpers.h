#ifndef SPIFFS_HELPERS_H
#define SPIFFS_HELPERS_H

#include "FsHelperBase.h"

#include <string>

class SpiffsHelper : public FsHelperBase {
public:
  SpiffsHelper(const int maxFiles                = 50,
               const std::string& basePath       = "/spiffs",
               const std::string& partitionLabel = "spiffs",
               const bool formatIfMountFailed    = true);

private:
  const int _maxFiles;
};
#endif // SPIFFS_HELPERS_H
