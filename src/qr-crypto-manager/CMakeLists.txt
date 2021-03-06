# Create library target and specify the implementation files to compile.
add_library(qr-crypto-manager 
    src/QRCryptoManager.cpp
)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

# Set target include directories 
#   These are the folders where the compiler will search for included header 
#   files.
#   The include files in the `include` folder are part of the public API of the
#   library, users of the library need to be able to include them in their code. 
#   Any header files in the `src` directory are private, they are only required 
#   to compile the implementation files of the library, users of the library
#   should not include them.
target_include_directories(qr-crypto-manager
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${OpenCV_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Tell CMake to compile this library with C++17 features enabled
target_compile_features(qr-crypto-manager PUBLIC cxx_std_17)

# Link the library with its dependencies.
#   This manages both the include directories of the dependencies, as well as  
#   the actual linking of the other libraries.
#   Libraries that are used in the public header files should be public,
#   libraries that are only used in the implementation files should be private.
target_link_libraries(qr-crypto-manager 
    PUBLIC
        comm
        baremetal-comm
    PRIVATE 
        ${OpenCV_LIBS}
        qr-decode
        base64
        crypto
        Threads::Threads
)

# Run CMake again in the `test` folder, to discover the CMakeLists.txt file that
# adds the tests.
# add_subdirectory(test) 