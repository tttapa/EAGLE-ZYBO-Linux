#pragma once

#include <cstdint>  // uint#_t
#include <cstring>  // memcpy

template <class T>
struct LoggableType {
    // static uint8_t getTypeID();
    // static size_t getLength();
    // static void writeToBuffer(const T &, uint8_t *buffer);
    // static T readFromBuffer(const uint8_t *buffer);
};

#define LOGGER_ADD_TRIVIAL_TYPE(type, typeid)                                  \
    template <>                                                                \
    struct LoggableType<type> {                                                \
        inline static uint8_t getTypeID() { return typeid; }                   \
        inline static size_t getLength() { return sizeof(type); }              \
        inline static void writeToBuffer(const type &t, uint8_t *buffer) {     \
            memcpy(buffer, &t, getLength());                                   \
        }                                                                      \
        inline static type readFromBuffer(const uint8_t *buffer) {             \
            type t;                                                            \
            memcpy(&t, buffer, getLength());                                   \
            return t;                                                          \
        }                                                                      \
    }

LOGGER_ADD_TRIVIAL_TYPE(int8_t, 1);
LOGGER_ADD_TRIVIAL_TYPE(uint8_t, 2);
LOGGER_ADD_TRIVIAL_TYPE(int16_t, 3);
LOGGER_ADD_TRIVIAL_TYPE(uint16_t, 4);
LOGGER_ADD_TRIVIAL_TYPE(int32_t, 5);
LOGGER_ADD_TRIVIAL_TYPE(uint32_t, 6);
LOGGER_ADD_TRIVIAL_TYPE(int64_t, 7);
LOGGER_ADD_TRIVIAL_TYPE(uint64_t, 8);
LOGGER_ADD_TRIVIAL_TYPE(float, 9);
LOGGER_ADD_TRIVIAL_TYPE(double, 10);
LOGGER_ADD_TRIVIAL_TYPE(bool, 11);
LOGGER_ADD_TRIVIAL_TYPE(char, 12);
