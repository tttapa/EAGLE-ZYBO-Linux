# Add an executable with tests, and specify the source files to compile
add_executable(crypto.test test-crypto.cpp)
# Link the test executable with the Google Test main entry point and the library
# under test
target_link_libraries(crypto.test gtest_main crypto comm)
target_include_directories(crypto.test PRIVATE ../src)

# Add the tests to Google Test
include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING AND FALSE) # Testing crypto on x64 is not very useful, I think?
gtest_discover_tests(crypto.test)
endif()

# # Add an executable with tests, and specify the source files to compile
# add_executable(crypto-pc.test test-crypto-pc.cpp)
# # Link the test executable with the Google Test main entry point and the library
# # under test
# target_link_libraries(crypto-pc.test gtest_main crypto)
# 
# # Add the tests to Google Test
# include(GoogleTest)
# if (NOT CMAKE_CROSSCOMPILING) # Testing crypto on x64 is not very useful, I think?
# gtest_discover_tests(crypto-pc.test)
# endif()