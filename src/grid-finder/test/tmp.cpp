#include "TestMask.hpp"
#include <GridFinder.hpp>
#include <PerfTimer.hpp>

#pragma optimize("", off)
int main() {
    Square square;
    constexpr size_t ITERATIONS = 512;
    PerfTimer pt;
    for (size_t i = 0; i < ITERATIONS; ++i) {
        GridFinder gf = {mask};
        square        = gf.findSquare();
    }
    float duration    = pt.getDuration<std::chrono::milliseconds>();
    float avgDuration = duration / ITERATIONS;
    float fps         = 1000.0 / avgDuration;
    cout << "Average duration: " << avgDuration << " ms\t"
         << "FPS: " << fps << std::endl;
}
#pragma optimize("", on)

#if 0
int main() {
    Square square;
    constexpr size_t ITERATIONS = 512;
    PerfTimer pt;
    for (volatile size_t i = 0; i < ITERATIONS; ++i) {
        // Don't optimize mask away
        mask[0][0]    = i > ITERATIONS / 2 ? 0xFF : 0x00;
        GridFinder gf = {mask};
        square        = gf.findSquare();
    }   
    float duration    = pt.getDuration<std::chrono::milliseconds>();
    float avgDuration = duration / ITERATIONS;
    float fps         = 1000.0 / avgDuration;
    cout << "Average duration: " << avgDuration << " ms\t"
         << "FPS: " << fps << std::endl;
}
#endif