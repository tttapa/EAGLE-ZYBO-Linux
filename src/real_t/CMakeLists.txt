# Create target and set properties
add_library(real_t INTERFACE)

#Set target properties
target_include_directories(real_t
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(TARGETS real_t
        EXPORT real_t
        INCLUDES DESTINATION include)
install(EXPORT real_t DESTINATION cmake)
install(
FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/real_t.h
DESTINATION include)