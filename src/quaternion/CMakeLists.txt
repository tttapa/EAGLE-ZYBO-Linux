add_library(quaternion INTERFACE)

target_include_directories(quaternion
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(quaternion 
    INTERFACE 
        matrix 
        utilities
)

if(NOT CMAKE_CROSSCOMPILING)
add_subdirectory(test) 
endif()

install(TARGETS quaternion
        EXPORT quaternion
        INCLUDES DESTINATION include)
install(EXPORT quaternion DESTINATION cmake)
install(
FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/EulerAngles.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Quaternion.hpp
DESTINATION include)