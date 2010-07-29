# Copyright (C) 2009 Sun Microsystems, Inc.
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

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

# Figure out additional libraries for use with -lmysql
FOREACH (dep ${libmysql_LIB_DEPENDS})
  STRING(REGEX MATCH "^-.*$" out "${dep}")
  IF(out)
    SET(extra_dynamic_LDFLAGS "${extra_dynamic_LDFLAGS} ${dep}")
  ENDIF(out)
ENDFOREACH(dep)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/mysql_config.c.in
               ${CMAKE_CURRENT_BINARY_DIR}/mysql_config.c @ONLY)

ADD_EXECUTABLE(mysql_config mysql_config.c)
TARGET_LINK_LIBRARIES(mysql_config mysqlclient)

# Installation
#
INSTALL(TARGETS mysql_config
        DESTINATION "bin")
