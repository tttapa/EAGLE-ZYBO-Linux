#include <LogEntry.hpp>

#include <iostream>

int main() {
    LogEntry l = {};
    std::cout << "Offset = " << +((uintptr_t) &l.rcInput.throttle - (uintptr_t) &l)
              << " bytes" << std::endl;
}