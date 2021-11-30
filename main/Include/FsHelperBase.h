#ifndef FS_HELPER_BASE_H
#define FS_HELPER_BASE_H

#include <fstream>
#include <memory>
#include <string>

class FsHelperBase {
public:
  FsHelperBase(const std::string& basePath,
               const std::string& partitionLabel,
               const bool formatIfMountFailed = true)
    : _basePath{basePath},
      _partitionLabel{partitionLabel},
      _formatIfMountFailed{formatIfMountFailed} {}

  int64_t WriteText(uint32_t numBytes);
  // void WriteBin(uint32_t numBytes);

  int64_t ReadText(int fileId);
  void DeleteFile(int fileId);

  void openFile(uint32_t fileIndex);
  void closeFile();
  void writeToFile(const char* data, size_t size);
  void readFromFile();

protected:
  const std::string& _basePath;
  const std::string& _partitionLabel;
  const bool _formatIfMountFailed;

private:
  uint32_t _fileIndex                           = 0;
  std::shared_ptr<std::fstream> _currFileStream = nullptr;

  std::string getNextName();
  std::string getPath(int fileId);
};
#endif // FS_HELPER_BASE_H
