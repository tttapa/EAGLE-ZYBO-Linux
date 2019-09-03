#pragma once

#include <cstddef>

inline size_t nextWord(size_t i) { return i - (i % 4) + 4; }
inline size_t roundUpToWordSizeMultiple(size_t i) {
    return i + 3 - ((i + 3) % 4);
}