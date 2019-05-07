#-------------------------- Code generator for logger -------------------------#
#------------------------------------------------------------------------------#

add_custom_command(
    OUTPUT
        src-generated/LogEntry.cpp
        src-generated/DroneLogger.py.c
        src-generated/logger.drone.cpp
        include-generated/LogEntry.h 
    COMMAND
        ${CMAKE_CURRENT_SOURCE_DIR}/Codegen.py ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS
        Codegen.py
        CodegenSnippets.py
        templates/LogEntry.template.cpp
        templates/DroneLogger.py.template.c
        templates/logger.drone.template.cpp
        templates/LogEntry.template.h
)

#------------------------------- Library target -------------------------------#
#------------------------------------------------------------------------------#

# Create library target and specify the implementation files to compile.
add_library(logentry
    src-generated/LogEntry.cpp
)

# Set target include directories 
#   These are the folders where the compiler will search for included header 
#   files.
#   The include files in the `include` folder are part of the public API of the
#   library, users of the library need to be able to include them in their code. 
#   Any header files in the `src` directory are private, they are only required 
#   to compile the implementation files of the library, users of the library
#   should not include them.
target_include_directories(logentry
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include-generated>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        src-generated
)

# Tell CMake to compile this library with C++17 features enabled
target_compile_features(logentry PUBLIC cxx_std_17)

# Link the library with its dependencies.
#   This manages both the include directories of the dependencies, as well as  
#   the actual linking of the other libraries.
#   Libraries that are used in the public header files should be public,
#   libraries that are only used in the implementation files should be private.
# target_link_libraries(logentry
#     PUBLIC
#         ...
#     PRIVATE
#         ...
# )

#--------------------------------- Unit tests ---------------------------------#
#------------------------------------------------------------------------------#

# Run CMake again in the `test` folder, to discover the CMakeLists.txt file that
# adds the tests.
# add_subdirectory(test) # TODO

#------------------------ Python Logger module for GUI ------------------------#
#------------------------------------------------------------------------------#

if(NOT CMAKE_CROSSCOMPILING)

set(DRONELOGGER_INCLUDE_DIR 
    include-generated)
set(DRONELOGGER_MODULE_SOURCE 
    src-generated/DroneLogger.py.c)
configure_file(setup.py.in setup.py @ONLY)

add_custom_target(PythonDroneLogger ALL
    COMMAND
        ${PYTHON_EXECUTABLE} setup.py bdist_wheel
    DEPENDS
        logentry
        src-generated/DroneLogger.py.c
)

install(DIRECTORY 
            ${CMAKE_CURRENT_BINARY_DIR}/dist
            ${CMAKE_CURRENT_BINARY_DIR}/DroneLogger.egg-info
        DESTINATION PythonDroneLogger)
install(CODE
    "execute_process(COMMAND
        \"${PYTHON_EXECUTABLE} -m pip install --user --upgrade 
        ${CMAKE_CURRENT_BINARY_DIR}/dist/*.whl\")")

endif()

#-------------------- Installation for use with Baremetal ---------------------#
#------------------------------------------------------------------------------#

install(TARGETS logentry
        EXPORT logentry
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include)
install(EXPORT logentry DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include-generated/LogEntry.h
        DESTINATION include)