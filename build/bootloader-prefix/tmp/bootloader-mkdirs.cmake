# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/bhargav/esp/esp-idf/components/bootloader/subproject"
  "/home/bhargav/esp/kode-dot/build/bootloader"
  "/home/bhargav/esp/kode-dot/build/bootloader-prefix"
  "/home/bhargav/esp/kode-dot/build/bootloader-prefix/tmp"
  "/home/bhargav/esp/kode-dot/build/bootloader-prefix/src/bootloader-stamp"
  "/home/bhargav/esp/kode-dot/build/bootloader-prefix/src"
  "/home/bhargav/esp/kode-dot/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/bhargav/esp/kode-dot/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/bhargav/esp/kode-dot/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
