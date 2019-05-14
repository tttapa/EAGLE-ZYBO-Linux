# Add an executable with tests, and specify the source files to compile
add_executable(mask.test 
    test-Mask.cpp
)

# Tests need access to private headers as well
target_include_directories(mask.test 
    PRIVATE 
        ../src
)

# Link the test executable with the Google Test main entry point and the library
# under test
target_link_libraries(mask.test 
    gtest_main
    mask
)

# Add the tests to Google Test
include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING)
gtest_discover_tests(mask.test)
endif()