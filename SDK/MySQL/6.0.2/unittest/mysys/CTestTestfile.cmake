# CMake generated Testfile for 
# Source directory: /tmp/bootstrap-dir.5535/source/unittest/mysys
# Build directory: /tmp/bootstrap-dir.5535/source/unittest/mysys
# 
# This file replicates the SUBDIRS() and ADD_TEST() commands from the source
# tree CMakeLists.txt file, skipping any SUBDIRS() or ADD_TEST() commands
# that are excluded by CMake control structures, i.e. IF() commands.
ADD_TEST(bitmap-t "/bitmap-t")
SET_TESTS_PROPERTIES(bitmap-t PROPERTIES  TIMEOUT "120")
ADD_TEST(base64-t "/base64-t")
SET_TESTS_PROPERTIES(base64-t PROPERTIES  TIMEOUT "120")
ADD_TEST(my_atomic-t "/my_atomic-t")
SET_TESTS_PROPERTIES(my_atomic-t PROPERTIES  TIMEOUT "120")
ADD_TEST(lf-t "/lf-t")
SET_TESTS_PROPERTIES(lf-t PROPERTIES  TIMEOUT "120")
ADD_TEST(waiting_threads-t "/waiting_threads-t")
SET_TESTS_PROPERTIES(waiting_threads-t PROPERTIES  TIMEOUT "120")
