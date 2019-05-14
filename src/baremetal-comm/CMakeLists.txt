# Create library target and specify the implementation files to compile.
add_library(baremetal-comm INTERFACE)

# Set target include directories 
#   These are the folders where the compiler will search for included header 
#   files.
#   The include files in the `include` folder are part of the public API of the
#   library, users of the library need to be able to include them in their code. 
#   Any header files in the `src` directory are private, they are only required 
#   to compile the implementation files of the library, users of the library
#   should not include them.
target_include_directories(baremetal-comm
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(baremetal-comm
    INTERFACE
        logentry
)

# Run CMake again in the `test` folder, to discover the CMakeLists.txt file that
# adds the tests.
add_subdirectory(test)

#-------------------- Installation for use with Baremetal ---------------------#
#------------------------------------------------------------------------------#

install(TARGETS baremetal-comm
        EXPORT baremetal-comm
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include)
install(EXPORT baremetal-comm DESTINATION lib)
install(FILES 
            ${CMAKE_CURRENT_SOURCE_DIR}/include/BaremetalCommunicationDef.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/SharedStruct.hpp
        DESTINATION include)