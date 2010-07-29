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

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/include/my_config.h.in
               ${CMAKE_SOURCE_DIR}/extlib/yassl/include/config.h)
ADD_DEFINITIONS("-DHAVE_CONFIG_H")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/extlib/yassl/include
                    ${CMAKE_SOURCE_DIR}/extlib/yassl/taocrypt/include
                    ${CMAKE_SOURCE_DIR}/extlib/yassl/taocrypt/mySTL
					${CMAKE_SOURCE_DIR}/include)

ADD_DEFINITIONS("-D_LIB -DYASSL_PREFIX")

SET(YASSL_SOURCES  src/buffer.cpp src/cert_wrapper.cpp src/crypto_wrapper.cpp src/handshake.cpp src/lock.cpp 
				src/log.cpp src/socket_wrapper.cpp src/ssl.cpp src/timer.cpp src/yassl_error.cpp 
				src/yassl_imp.cpp src/yassl_int.cpp)

SET_SOURCE_FILES_PROPERTIES(${YASSL_SOURCES} LANGUAGE C)

IF(NOT SOURCE_SUBLIBS)
  ADD_LIBRARY(yassl ${YASSL_SOURCES})
ENDIF(NOT SOURCE_SUBLIBS)
