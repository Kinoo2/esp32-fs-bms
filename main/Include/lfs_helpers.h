#ifndef LFS_HELPERS_H
#define LFS_HELPERS_H

#include "FsHelperBase.h"

#include <string>

class LfsHelper : public FsHelperBase {
public:
  LfsHelper(const std::string& basePath       = "/lfs",
            const std::string& partitionLabel = "littlefs",
            const bool formatIfMountFailed    = true,
            const bool dontMount              = false);

private:
  const bool _dontMount;
};

#endif // LFS_HELPERS_H
