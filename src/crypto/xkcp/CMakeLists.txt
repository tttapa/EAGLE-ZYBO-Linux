add_library(cryptosw
    EagleCryptSW.c)

target_include_directories(cryptosw
    PUBLIC
        .
    PRIVATE
        EAGLE-Hash.a.headers
        EAGLE-KetjeJr.a.headers
)

target_link_libraries(cryptosw
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/EAGLE-Hash.a
        ${CMAKE_CURRENT_SOURCE_DIR}/EAGLE-KetjeJr.a
)