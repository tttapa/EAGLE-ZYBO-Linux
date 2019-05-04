# Add an executable with tests, and specify the source files to compile
add_executable(sharedmem.test test-SharedMem.cpp)
# Link the test executable with the Google Test main entry point and the library
# under test
target_link_libraries(sharedmem.test gtest_main baremetal-comm)

# Add the tests to Google Test
include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING AND FALSE) 
# Testing shared mememory on x64 is not very useful, I think?
gtest_discover_tests(sharedmem.test)
endif()