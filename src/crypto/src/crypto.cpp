#include <crypto.hpp>
#include <wrapper.h>

constexpr uintptr_t R0 = 0x43c70000;
constexpr uintptr_t R1 = R0 + 4;

void writeR0(uint32_t value) { wmem(R0, value); }
uint32_t readR0() { return rmem(R0); }
uint32_t readR1() { return rmem(R1); }