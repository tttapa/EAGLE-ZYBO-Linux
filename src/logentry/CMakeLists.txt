#-------------------------- Code generator for logger -------------------------#
#------------------------------------------------------------------------------#

add_custom_command(
    OUTPUT
        # ${CMAKE_CURRENT_BINARY_DIR}/src-generated/GetLogEntry.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/include-generated/LogEntry.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/include-generated/LoggerStructs.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/src-generated/DroneLogEntry.Python.cpp
    COMMAND
        ${CMAKE_CURRENT_SOURCE_DIR}/Codegen.py
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/Codegen.py
        ${CMAKE_CURRENT_SOURCE_DIR}/Codegen.json
)

add_custom_target(logentry-headers 
    DEPENDS 
        ${CMAKE_CURRENT_BINARY_DIR}/include-generated/LogEntry.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/include-generated/LoggerStructs.hpp
)

#------------------------------- Library target -------------------------------#
#------------------------------------------------------------------------------#

# Create a header-only library target
add_library(logentry INTERFACE)
add_dependencies(logentry logentry-headers)

# Set target include directories 
#   These are the folders where the compiler will search for included header 
#   files.
#   The include files in the `include` folder are part of the public API of the
#   library, users of the library need to be able to include them in their code. 
#   Any header files in the `src` directory are private, they are only required 
#   to compile the implementation files of the library, users of the library
#   should not include them.
target_include_directories(logentry
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include-generated>
)

# Link the library with its dependencies.
#   This manages both the include directories of the dependencies, as well as  
#   the actual linking of the other libraries.
#   Libraries that are used in the public header files should be public,
#   libraries that are only used in the implementation files should be private.
target_link_libraries(logentry
    INTERFACE
        quaternion
)

#------------------------ Python Logger module for GUI ------------------------#
#------------------------------------------------------------------------------#

if(NOT CMAKE_CROSSCOMPILING)

find_package(pybind11 REQUIRED)

# Create a Python module
pybind11_add_module(DroneLogger 
    ${CMAKE_CURRENT_BINARY_DIR}/src-generated/DroneLogEntry.Python.cpp
    EXCLUDE_FROM_ALL)

# Set target include directories 
#   These are the folders where the compiler will search for included header 
#   files.
#   The include files in the `include` folder are part of the public API of the
#   library, users of the library need to be able to include them in their code. 
target_include_directories(DroneLogger
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include-generated>
    PRIVATE
        # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src-generated>
)

# Link the library with its dependencies.
#   This manages both the include directories of the dependencies, as well as  
#   the actual linking of the other libraries.
target_link_libraries(DroneLogger
    PUBLIC
        logentry
        pybind11::module
        py-matrix
)

endif()

#-------------------- Installation for use with Baremetal ---------------------#
#------------------------------------------------------------------------------#

install(TARGETS logentry
        EXPORT logentry
        INCLUDES DESTINATION include)
install(EXPORT logentry DESTINATION lib)
install(FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/include-generated/LogEntry.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/include-generated/LoggerStructs.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/GetLogData.hpp
        DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/src-generated/GetLogData.cpp
        DESTINATION src)