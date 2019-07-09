#include <LogEntry.hpp>

#include <iostream>

int main() {
    LogEntry l = {};
    std::cout << "Offset = " << +((uintptr_t) &l.altitudeMeasurement - (uintptr_t) &l)
              << " bytes" << std::endl;
}