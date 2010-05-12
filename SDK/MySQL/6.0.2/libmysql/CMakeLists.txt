# Copyright (C) 2006 MySQL AB
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
INCLUDE("${PROJECT_SOURCE_DIR}/win/mysql_manifest.cmake")


# Note that we don't link with the libraries "strings" or "mysys"
# here, instead we recompile the files needed and include them
# directly. This means we don't have to worry here about if these
# libraries are compiled defining USE_TLS or not. Not that it *should*
# have been a problem anyway, they don't use thread local storage.

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include 
                    ${CMAKE_SOURCE_DIR}/libmysql
                    ${CMAKE_SOURCE_DIR}/regex
                    ${CMAKE_SOURCE_DIR}/sql
                    ${CMAKE_SOURCE_DIR}/strings)

# We include the source file listing instead of referencing the
# libraries. At least with CMake 2.4 and Visual Studio 2005 a static
# library created from other static libraries would not be complete,
# i.e. the libraries listed in TARGET_LINK_LIBRARIES() were just
# ignored.


# Include and add the directory path
SET(SOURCE_SUBLIBS TRUE)
SET(LIB_SOURCES "")

IF(NOT WITH_EXTERNAL_ZLIB)
  INCLUDE(${CMAKE_SOURCE_DIR}/extlib/zlib/CMakeLists.txt)
  FOREACH(rpath ${ZLIB_SOURCES})
    SET(LIB_SOURCES ${LIB_SOURCES} ../extlib/zlib/${rpath})
  ENDFOREACH(rpath)
ENDIF(NOT WITH_EXTERNAL_ZLIB)

INCLUDE(${CMAKE_SOURCE_DIR}/extlib/dbug/CMakeLists.txt)
FOREACH(rpath ${DBUG_SOURCES})
  SET(LIB_SOURCES ${LIB_SOURCES} ../extlib/dbug/${rpath})
ENDFOREACH(rpath)

IF(NOT SKIP_SSL AND NOT WITH_OPENSSL)
  INCLUDE(${CMAKE_SOURCE_DIR}/extlib/yassl/taocrypt/CMakeLists.txt)
  FOREACH(rpath ${TAOCRYPT_SOURCES})
    SET(LIB_SOURCES ${LIB_SOURCES} ../extlib/yassl/taocrypt/${rpath})
  ENDFOREACH(rpath)

  INCLUDE(${CMAKE_SOURCE_DIR}/extlib/yassl/CMakeLists.txt)
  FOREACH(rpath ${YASSL_SOURCES})
    SET(LIB_SOURCES ${LIB_SOURCES} ../extlib/yassl/${rpath})
  ENDFOREACH(rpath)
ENDIF(NOT SKIP_SSL AND NOT WITH_OPENSSL)

INCLUDE(${CMAKE_SOURCE_DIR}/mysys/CMakeLists.txt)
FOREACH(rpath ${MYSYS_SOURCES})
  SET(LIB_SOURCES ${LIB_SOURCES} ../mysys/${rpath})
ENDFOREACH(rpath)

INCLUDE(${CMAKE_SOURCE_DIR}/strings/CMakeLists.txt)
FOREACH(rpath ${STRINGS_SOURCES})
  SET(LIB_SOURCES ${LIB_SOURCES} ../strings/${rpath})
ENDFOREACH(rpath)

INCLUDE(${CMAKE_SOURCE_DIR}/vio/CMakeLists.txt)
FOREACH(rpath ${VIO_SOURCES})
  SET(LIB_SOURCES ${LIB_SOURCES} ../vio/${rpath})
ENDFOREACH(rpath)

SET(CLIENT_SOURCES  client.c errmsg.c get_password.c libmysql.c
                    my_time.c net_serv.c pack.c password.c 
		    ${LIB_SOURCES})

ADD_LIBRARY(mysqlclient       STATIC ${CLIENT_SOURCES})
TARGET_LINK_LIBRARIES(mysqlclient ${CMAKE_THREAD_LIBS_INIT})
IF(MATH_LIBRARY)
  TARGET_LINK_LIBRARIES(mysqlclient ${MATH_LIBRARY})
ENDIF(MATH_LIBRARY)
IF(OPENSSL_LIBRARIES)
  TARGET_LINK_LIBRARIES(mysqlclient ${OPENSSL_LIBRARIES} ${OPENSSL_LIBCRYPTO})
ENDIF(OPENSSL_LIBRARIES)
IF(SOCKET_LIBRARY)
  TARGET_LINK_LIBRARIES(mysqlclient ${SOCKET_LIBRARY})
ENDIF(SOCKET_LIBRARY)
TARGET_LINK_LIBRARIES(mysqlclient ${ZLIB_LIBRARIES})

ADD_LIBRARY(libmysql          SHARED ${CLIENT_SOURCES} libmysql.def)
TARGET_LINK_LIBRARIES(libmysql ${CMAKE_THREAD_LIBS_INIT})
STRING(REGEX REPLACE "\\..+" "" LIBMYSQL_SOVERSION ${SHARED_LIB_VERSION})
SET_TARGET_PROPERTIES(libmysql
                      PROPERTIES VERSION ${SHARED_LIB_VERSION}
                                 SOVERSION ${LIBMYSQL_SOVERSION})
IF(OPENSSL_LIBRARIES)
  TARGET_LINK_LIBRARIES(libmysql ${OPENSSL_LIBRARIES} ${OPENSSL_LIBCRYPTO})
ENDIF(OPENSSL_LIBRARIES)
IF(SOCKET_LIBRARY)
  TARGET_LINK_LIBRARIES(libmysql ${SOCKET_LIBRARY})
ENDIF(SOCKET_LIBRARY)
TARGET_LINK_LIBRARIES(libmysql ${ZLIB_LIBRARIES})
IF(MATH_LIBRARY)
  TARGET_LINK_LIBRARIES(libmysql ${MATH_LIBRARY})
ENDIF(MATH_LIBRARY)

IF(NOT WIN32)
  ADD_CUSTOM_COMMAND(OUTPUT "libmysqlclient${CMAKE_SHARED_LIBRARY_SUFFIX}"
                            "libmysqlclient_r${CMAKE_SHARED_LIBRARY_SUFFIX}"
                    COMMAND ${CMAKE_COMMAND} ARGS -E remove -f libmysqlclient${CMAKE_SHARED_LIBRARY_SUFFIX}
                    COMMAND ${CMAKE_COMMAND} ARGS -E create_symlink libmysql${CMAKE_SHARED_LIBRARY_SUFFIX} libmysqlclient${CMAKE_SHARED_LIBRARY_SUFFIX}
                    COMMAND ${CMAKE_COMMAND} ARGS -E remove -f libmysqlclient_r${CMAKE_SHARED_LIBRARY_SUFFIX}
                    COMMAND ${CMAKE_COMMAND} ARGS -E create_symlink libmysql${CMAKE_SHARED_LIBRARY_SUFFIX} libmysqlclient_r${CMAKE_SHARED_LIBRARY_SUFFIX}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/libmysql
                    DEPENDS libmysql)

  ADD_CUSTOM_TARGET(LIBMYSQL_SYMLINKS
                    ALL
                    DEPENDS "libmysqlclient${CMAKE_SHARED_LIBRARY_SUFFIX}"
                            "libmysqlclient_r${CMAKE_SHARED_LIBRARY_SUFFIX}")
ENDIF(NOT WIN32)

IF(NOT WIN32)
  # Avoid a redundant 'lib'
  SET_TARGET_PROPERTIES(libmysql
                        PROPERTIES PREFIX "")
ENDIF(NOT WIN32)

IF(EMBED_MANIFESTS)
  MYSQL_EMBED_MANIFEST("myTest" "asInvoker")
ENDIF(EMBED_MANIFESTS)

# Installation
#
IF(WIN32)
  INSTALL(TARGETS libmysql mysqlclient
          RUNTIME DESTINATION "lib"
          LIBRARY DESTINATION "lib"
          ARCHIVE DESTINATION "lib")
ELSE(WIN32)
  INSTALL(TARGETS libmysql mysqlclient
          LIBRARY DESTINATION "lib"
          ARCHIVE DESTINATION "lib")
  INSTALL(FILES ${CMAKE_SOURCE_DIR}/libmysql/libmysqlclient${CMAKE_SHARED_LIBRARY_SUFFIX}
                ${CMAKE_SOURCE_DIR}/libmysql/libmysqlclient_r${CMAKE_SHARED_LIBRARY_SUFFIX}
          DESTINATION "lib")
ENDIF(WIN32)
