# Copyright (C) 2008 Sun Microsystems, Inc.
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

ENABLE_TESTING()

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSAFEMALLOC -DSAFE_MUTEX")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSAFEMALLOC -DSAFE_MUTEX")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include
                    ${CMAKE_SOURCE_DIR}/unittest/mytap)

SET(API_TESTS "basic-t" "fetch" "charset" "logs" "errors" "cursor" "view" "ps" "ps_bugs" 
              "sp" "result" "connection" "misc")

FOREACH(API_TEST ${API_TESTS})
  ADD_EXECUTABLE(${API_TEST} ${API_TEST}.c)
  TARGET_LINK_LIBRARIES(${API_TEST} mytap mysqlclient)
  ADD_TEST(${API_TEST} ${EXECUTABLE_OUTPUT_PATH}/${API_TEST})
  SET_TESTS_PROPERTIES(${API_TEST} PROPERTIES TIMEOUT 120)
ENDFOREACH(API_TEST)
