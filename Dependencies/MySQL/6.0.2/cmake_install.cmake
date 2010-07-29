# Install script for directory: /tmp/bootstrap-dir.5535/source

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local/mysql")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/tmp/bootstrap-dir.5535/source/include/" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE OPTIONAL FILES
    "/tmp/bootstrap-dir.5535/source/README"
    "/tmp/bootstrap-dir.5535/source/COPYING"
    "/tmp/bootstrap-dir.5535/source/LICENSE.mysql"
    "/tmp/bootstrap-dir.5535/source/EXCEPTIONS-CLIENT"
    "/tmp/bootstrap-dir.5535/source/ChaneLog"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/tmp/bootstrap-dir.5535/source/vio/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/extlib/dbug/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/strings/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/extlib/regex/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/extlib/zlib/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/mysys/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/extlib/yassl/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/extlib/yassl/taocrypt/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/libmysql/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/tests/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/unittest/mytap/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/unittest/examples/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/unittest/libmysql/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/unittest/mysys/cmake_install.cmake")
  INCLUDE("/tmp/bootstrap-dir.5535/source/mysql_config/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/tmp/bootstrap-dir.5535/source/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/tmp/bootstrap-dir.5535/source/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
