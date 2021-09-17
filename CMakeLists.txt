# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(COMPONENTS
  esp-common
  esptool_py
  main

  esp_littlefs
  spiffs
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
add_compile_options(-fdiagnostics-color=always)
set(ENV{GCC_COLORS} "error=01;31:warning=01;33:note=01;36:caret=01;32:locus=01:quote=01")
project(template-app)
