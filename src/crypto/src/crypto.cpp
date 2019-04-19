#include <crypto.hpp>
#include <wrapper.h>

#include <registers.hpp>

void writeR0(uint32_t value) { wmem(R0_ADDR, value); }
uint32_t readR0() { return rmem(R0_ADDR); }
uint32_t readR1() { return rmem(R1_ADDR); }