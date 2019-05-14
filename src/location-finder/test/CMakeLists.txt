# Add an executable with tests, and specify the source files to compile
add_executable(location_finder.test 
    test-LocationFinder.cpp
)

# Tests need access to private headers as well
target_include_directories(location_finder.test 
    PRIVATE 
        ../src
)

# Link the test executable with the Google Test main entry point and the library
# under test
target_link_libraries(location_finder.test 
    gtest_main
    location_finder
)

# Add the tests to Google Test
include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING)
gtest_discover_tests(location_finder.test)
endif()