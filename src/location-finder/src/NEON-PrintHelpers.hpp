#pragma once

#include <arm_neon.h>
#include <ostream>

std::ostream &operator<<(std::ostream &os, int8x16_t x) {
    os << "[ ";
    os << +vgetq_lane_s8(x, 0) << ' ';
    os << +vgetq_lane_s8(x, 1) << ' ';
    os << +vgetq_lane_s8(x, 2) << ' ';
    os << +vgetq_lane_s8(x, 3) << ' ';
    os << +vgetq_lane_s8(x, 4) << ' ';
    os << +vgetq_lane_s8(x, 5) << ' ';
    os << +vgetq_lane_s8(x, 6) << ' ';
    os << +vgetq_lane_s8(x, 7) << ' ';
    os << +vgetq_lane_s8(x, 8) << ' ';
    os << +vgetq_lane_s8(x, 9) << ' ';
    os << +vgetq_lane_s8(x, 10) << ' ';
    os << +vgetq_lane_s8(x, 11) << ' ';
    os << +vgetq_lane_s8(x, 12) << ' ';
    os << +vgetq_lane_s8(x, 13) << ' ';
    os << +vgetq_lane_s8(x, 14) << ' ';
    os << +vgetq_lane_s8(x, 15) << ' ';
    return os << ']';
}

std::ostream &operator<<(std::ostream &os, uint8x16_t x) {
    os << "[ ";
    os << +vgetq_lane_u8(x, 0) << ' ';
    os << +vgetq_lane_u8(x, 1) << ' ';
    os << +vgetq_lane_u8(x, 2) << ' ';
    os << +vgetq_lane_u8(x, 3) << ' ';
    os << +vgetq_lane_u8(x, 4) << ' ';
    os << +vgetq_lane_u8(x, 5) << ' ';
    os << +vgetq_lane_u8(x, 6) << ' ';
    os << +vgetq_lane_u8(x, 7) << ' ';
    os << +vgetq_lane_u8(x, 8) << ' ';
    os << +vgetq_lane_u8(x, 9) << ' ';
    os << +vgetq_lane_u8(x, 10) << ' ';
    os << +vgetq_lane_u8(x, 11) << ' ';
    os << +vgetq_lane_u8(x, 12) << ' ';
    os << +vgetq_lane_u8(x, 13) << ' ';
    os << +vgetq_lane_u8(x, 14) << ' ';
    os << +vgetq_lane_u8(x, 15) << ' ';
    return os << ']';
}