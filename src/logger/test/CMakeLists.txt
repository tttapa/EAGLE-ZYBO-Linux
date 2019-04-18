# Add an executable with tests, and specify the source files to compile
add_executable(test-logger test-multicast.cpp)
# Link the test executable with the Google Test main entry point and the library
# under test
target_link_libraries(test-logger gtest_main logger)

# Add the tests to Google Test
include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING)
gtest_discover_tests(test-logger)
endif()