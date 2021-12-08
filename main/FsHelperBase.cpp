#include "FsHelperBase.h"

#include "lipsum.h"

#include <cstring>
#include <errno.h>
#include <esp_vfs.h>

using namespace std;

string FsHelperBase::getPath(int fileId) {
  uint32_t width = 4;
  char filenameStr[width + 1];
  snprintf(filenameStr, width + 1, "%0*d", width, fileId);
  return _basePath + "/" + string{filenameStr};
}

void FsHelperBase::openFile(uint32_t fileIndex) {
  auto path = getPath(fileIndex);
  struct stat st;
  stat(path.c_str(), &st);
  printf("stat len=%d\n", (int)st.st_size);

  if (_currFileStream == nullptr) {
    _currFileStream = make_shared<fstream>();
  }

  if (!_currFileStream->is_open()) {
    // Open file in "append" mode.
    _currFileStream->open(path.c_str(),
                          fstream::in | fstream::out | fstream::app);
    auto st  = _currFileStream->rdstate();
    auto len = _currFileStream->tellp();
    printf("_currFileStream->rdstate()=%d, len=%d\n", st, (int)len);

    _currFileStream->seekp(0, fstream::end);
    st  = _currFileStream->rdstate();
    len = _currFileStream->tellp();
    printf("_currFileStream->rdstate()=%d, len=%d\n", st, (int)len);

    _fileIndex = fileIndex;
    printf("Opening file: %s\n", path.c_str());
  }
}

void FsHelperBase::closeFile() {
  printf("Closing file: %s\n", getPath(_fileIndex).c_str());
  if (_currFileStream != nullptr && _currFileStream->is_open()) {
    _currFileStream->flush();
    _currFileStream->close();
  }
  _currFileStream.reset();
}

void FsHelperBase::writeToFile(const char* data, size_t size) {
  if (_currFileStream == nullptr || !_currFileStream->is_open()) {
    printf("ERROR: No file open!");
    return;
  }

  printf("Writing %d bytes to %s.\n", size, getPath(_fileIndex).c_str());
  _currFileStream->write(data, size);
  _currFileStream->flush();
}

void FsHelperBase::readFromFile() {
  if (_currFileStream == nullptr || !_currFileStream->is_open()) {
    printf("ERROR: No file open!");
    return;
  }

  _currFileStream->seekp(0);

  char* data = (char*)malloc(10000);
  auto len   = _currFileStream->readsome(data, 10000);
  printf("Reading %d bytes from %s.\n", len, getPath(_fileIndex).c_str());

  data[len] = '\0';
  printf("%s\n", data);
}
