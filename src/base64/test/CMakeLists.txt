# Add an executable with tests, and specify the source files to compile
add_executable(base64.test test-base64.cpp)
# Link the test executable with the Google Test main entry point and the library
# under test
target_link_libraries(base64.test gtest_main base64)

# Add the tests to Google Test
include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING)
gtest_discover_tests(base64.test)
endif()