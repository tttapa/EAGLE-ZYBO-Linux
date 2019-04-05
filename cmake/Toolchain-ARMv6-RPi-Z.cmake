# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER   arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

# TODO
# where is the target environment
SET(CMAKE_FIND_ROOT_PATH  "${HOME}/sysroot-glibc-8.3-2019.03-x86_64-arm-linux-gnueabi")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-march=armv6 -mfpu=vfp -mfloat-abi=hard -marm"
CACHE STRING "" FORCE)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-march=armv6 -mfpu=vfp -mfloat-abi=hard -marm"
CACHE STRING "" FORCE)

# Link all libraries statically
SET(CMAKE_EXE_LINKER_FLAGS "-march=armv6 -mfpu=vfp -mfloat-abi=hard -marm -static"
CACHE STRING "" FORCE)