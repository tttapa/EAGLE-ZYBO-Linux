# Create target and set properties
add_library(matrix INTERFACE)

# Set target properties
target_include_directories(matrix
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_subdirectory(test)