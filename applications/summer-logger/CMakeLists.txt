# Add a new executable that is built from the given source file(s)
add_executable(summer-logger-app summer-logger.cpp)

# Link the necessary libraries to the application
target_link_libraries(summer-logger-app
    logger
    utilities
    summer-logger
)