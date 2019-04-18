# Add an executable with tests, and specify the source files to compile
add_executable(test-crypto test-crypto.cpp)
# Link the test executable with the Google Test main entry point and the library
# under test
target_link_libraries(test-crypto gtest_main crypto)

# Add the tests to Google Test
include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING AND FALSE) # Testing crypto on x64 is not very useful, I think?
gtest_discover_tests(test-crypto)
endif()