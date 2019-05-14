add_library(cryptosw
    EagleCryptSW.cpp)

target_include_directories(cryptosw
    PUBLIC
        .
    PRIVATE
        EAGLE-Hash.a.headers
        EAGLE-KetjeJr.a.headers
)

if (CMAKE_CROSSCOMPILING)
target_link_libraries(cryptosw
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib-arm/EAGLE-Hash.a
        ${CMAKE_CURRENT_SOURCE_DIR}/lib-arm/EAGLE-KetjeJr.a
)
else()
target_link_libraries(cryptosw
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib-x86/EAGLE-Hash.a
        ${CMAKE_CURRENT_SOURCE_DIR}/lib-x86/EAGLE-KetjeJr.a
)
endif()