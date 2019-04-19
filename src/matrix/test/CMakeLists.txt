file(GLOB TEST_SRCS *.cpp)
add_executable(matrix.test ${TEST_SRCS})
target_link_libraries(matrix.test gtest_main matrix)

include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING)
gtest_discover_tests(matrix.test)
endif()