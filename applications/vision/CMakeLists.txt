# Add a new executable that is built from the given source file(s)
add_executable(vision vision.cpp)

# Link the necessary libraries to the application
target_link_libraries(vision
    location_finder
    utilities
    comm
    baremetal-comm
)