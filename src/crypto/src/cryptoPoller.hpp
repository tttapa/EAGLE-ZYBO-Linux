#pragma once

#include <cstdint>
#include <wrapper.h>

#include <PerfTimer.hpp>
#include <cryptoException.hpp>
#include <registers.hpp>

class CryptoPoller {

  public:
    CryptoPoller(unsigned int timeoutMilliseconds = 100);
    uint32_t execute(uint32_t instruction);

  private:
    static bool isInitialized;
    static uint32_t currentBitFlip;
    unsigned int timeoutMilliseconds;

    static void writeR0(uint32_t value) { wmem(R0_ADDR, value); }
    static uint32_t readR0() { return rmem(R0_ADDR); }
    static uint32_t readR1() { return rmem(R1_ADDR); }

    uint32_t poll(uint32_t mask, uint32_t previousValue);
    void initialize();
};