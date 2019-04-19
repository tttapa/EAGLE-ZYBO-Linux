add_executable(utilities.test test-MeanSquareError.cpp)
target_link_libraries(utilities.test gtest_main Utilities::utilities)

include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING)
gtest_discover_tests(utilities.test)
endif()