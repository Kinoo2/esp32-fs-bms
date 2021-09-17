#ifndef SPIFFS_HELPERS_H
#define SPIFFS_HELPERS_H
#include <string>

class SpiffsHelper {
public:
  SpiffsHelper(const int maxFiles                = 50,
               const std::string& basePath       = "/spiffs",
               const std::string& partitionLabel = "spiffs",
               const bool formatIfMountFailed    = true);

private:
  const int _maxFiles;
  const std::string& _basePath;
  const std::string& _partitionLabel;
  const bool _formatIfMountFailed;
};
#endif // SPIFFS_HELPERS_H
