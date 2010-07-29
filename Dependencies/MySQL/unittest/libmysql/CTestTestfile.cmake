# CMake generated Testfile for 
# Source directory: /tmp/bootstrap-dir.5535/source/unittest/libmysql
# Build directory: /tmp/bootstrap-dir.5535/source/unittest/libmysql
# 
# This file replicates the SUBDIRS() and ADD_TEST() commands from the source
# tree CMakeLists.txt file, skipping any SUBDIRS() or ADD_TEST() commands
# that are excluded by CMake control structures, i.e. IF() commands.
ADD_TEST(basic-t "/basic-t")
SET_TESTS_PROPERTIES(basic-t PROPERTIES  TIMEOUT "120")
ADD_TEST(fetch "/fetch")
SET_TESTS_PROPERTIES(fetch PROPERTIES  TIMEOUT "120")
ADD_TEST(charset "/charset")
SET_TESTS_PROPERTIES(charset PROPERTIES  TIMEOUT "120")
ADD_TEST(logs "/logs")
SET_TESTS_PROPERTIES(logs PROPERTIES  TIMEOUT "120")
ADD_TEST(errors "/errors")
SET_TESTS_PROPERTIES(errors PROPERTIES  TIMEOUT "120")
ADD_TEST(cursor "/cursor")
SET_TESTS_PROPERTIES(cursor PROPERTIES  TIMEOUT "120")
ADD_TEST(view "/view")
SET_TESTS_PROPERTIES(view PROPERTIES  TIMEOUT "120")
ADD_TEST(ps "/ps")
SET_TESTS_PROPERTIES(ps PROPERTIES  TIMEOUT "120")
ADD_TEST(ps_bugs "/ps_bugs")
SET_TESTS_PROPERTIES(ps_bugs PROPERTIES  TIMEOUT "120")
ADD_TEST(sp "/sp")
SET_TESTS_PROPERTIES(sp PROPERTIES  TIMEOUT "120")
ADD_TEST(result "/result")
SET_TESTS_PROPERTIES(result PROPERTIES  TIMEOUT "120")
ADD_TEST(connection "/connection")
SET_TESTS_PROPERTIES(connection PROPERTIES  TIMEOUT "120")
ADD_TEST(misc "/misc")
SET_TESTS_PROPERTIES(misc PROPERTIES  TIMEOUT "120")
