# Add an executable with tests, and specify the source files to compile
add_executable(grid_finder.test 
    # test-GridFinder.cpp # // TODO
    test-Bresenham.cpp
    test-Angle.cpp
    test-Line.cpp
    test-CenterPointOutLineIterator.cpp
)
# Link the test executable with the Google Test main entry point and the library
# under test
target_link_libraries(grid_finder.test gtest_main grid_finder)

# Add the tests to Google Test
include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING)
gtest_discover_tests(grid_finder.test)
endif()