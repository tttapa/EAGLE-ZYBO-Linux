# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER   arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

# TODO
# where is the target environment
# SET(CMAKE_FIND_ROOT_PATH  /opt/)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-mcpu=cortex-a9 \
-mfpu=neon -mfloat-abi=hard -ftree-vectorize -mvectorize-with-neon-quad" 
CACHE STRING "" FORCE)
# -mfpu=neon-vfpv3")

# Link all libraries statically
SET(CMAKE_EXE_LINKER_FLAGS " -static"
CACHE STRING "" FORCE)