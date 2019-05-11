# Add an executable with tests, and specify the source files to compile
if(CMAKE_CROSSCOMPILING)
add_executable(crypto.test test-crypto.cpp test-crypto-PC.cpp)
else()
add_executable(crypto.test test-crypto-PC.cpp)
endif()
# Link the test executable with the Google Test main entry point and the library
# under test
target_link_libraries(crypto.test gtest_main crypto comm cryptosw)
target_include_directories(crypto.test PRIVATE ../src)

# Add the tests to Google Test
include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING)
gtest_discover_tests(crypto.test)
endif()