#pragma once

#include <chrono>

class PerfTimer {
  public:
    PerfTimer() { reset(); }
    decltype(std::chrono::high_resolution_clock::now()) start;
    template <class ToDuration = std::chrono::microseconds>
    auto getDuration() -> decltype(ToDuration{}.count()) {
        auto stop = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<ToDuration>(stop - start).count();
    }
    void reset() { start = std::chrono::high_resolution_clock::now(); }
};