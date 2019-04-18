SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

# Specify the cross compiler
SET(CMAKE_C_COMPILER   arm-cortexa9_neon-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER arm-cortexa9_neon-linux-gnueabihf-g++)

# Search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-ftree-vectorize -mvectorize-with-neon-quad \
-DREAL_RMEM_WMEM" # TODO
CACHE STRING "" FORCE)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-ftree-vectorize -mvectorize-with-neon-quad \
-DREAL_RMEM_WMEM" # TODO
CACHE STRING "" FORCE)