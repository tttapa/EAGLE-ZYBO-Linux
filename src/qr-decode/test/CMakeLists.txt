# Add an executable with tests, and specify the source files to compile
add_executable(qr_decode.test 
    test-QR-Decode.cpp
)

# Tests need access to private headers as well
# target_include_directories(qr_decode.test 
#     PRIVATE 
#         ../src
# )

# Link the test executable with the Google Test main entry point and the library
# under test
target_link_libraries(qr_decode.test 
    gtest_main
    qr-decode
    utilities
)

# Add the tests to Google Test
include(GoogleTest)
if (NOT CMAKE_CROSSCOMPILING)
gtest_discover_tests(qr_decode.test)
endif()