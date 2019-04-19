# Add an executable with tests, and specify the source files to compile
add_executable(logger.test test-multicast.cpp)
# Link the test executable with the Google Test main entry point and the library
# under test
target_link_libraries(logger.test gtest_main logger)

# Add the tests to Google Test
include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING)
gtest_discover_tests(logger.test)
endif()