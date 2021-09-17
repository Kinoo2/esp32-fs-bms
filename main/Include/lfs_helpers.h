#ifndef LFS_HELPERS_H
#define LFS_HELPERS_H
#include <string>

class LfsHelper {
public:
  LfsHelper(const std::string& basePath       = "/lfs",
            const std::string& partitionLabel = "littlefs",
            const bool formatIfMountFailed    = true,
            const bool dontMount              = false);

private:
  const std::string& _basePath;
  const std::string& _partitionLabel;
  const bool _formatIfMountFailed;
  const bool _dontMount;
};

#endif // LFS_HELPERS_H
