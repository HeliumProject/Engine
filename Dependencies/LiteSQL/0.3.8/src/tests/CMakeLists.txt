add_executable(test-selectquery test-selectquery.cpp)
target_link_libraries (test-selectquery litesql litesql-util)
add_test(test-selectquery ${EXECUTABLE_OUTPUT_PATH}/test-selectquery selectquery)

add_executable(test-split test-split.cpp)
target_link_libraries (test-split litesql-util)
add_test(test-split ${EXECUTABLE_OUTPUT_PATH}/test-split split)

add_executable(test-datetime test-datetime.cpp)
target_link_libraries (test-datetime litesql litesql-util)
add_test(test-datetime ${EXECUTABLE_OUTPUT_PATH}/test-datetime datetime)

add_executable(test-update-table test-update-table.cpp)
target_link_libraries (test-update-table litesql litesql-util ${BACKEND_LIBRARIES})
add_test(test-update-table ${EXECUTABLE_OUTPUT_PATH}/test-update-table update-table)

INCLUDE_DIRECTORIES(../library)
