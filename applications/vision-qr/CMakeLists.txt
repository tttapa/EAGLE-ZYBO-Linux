# Add a new executable that is built from the given source file(s)
add_executable(vision-qr visionqr.cpp)

# Link the necessary libraries to the application
target_link_libraries(vision-qr
    location_finder
    utilities
    qr-crypto-manager
    logger
)