add_executable(quaternion.test test-Quaternion.cpp)
target_link_libraries(quaternion.test gtest_main quaternion)

include(GoogleTest)
gtest_discover_tests(quaternion.test)