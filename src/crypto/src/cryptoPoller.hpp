#pragma once

#include <cstdint>
#include <wrapper.h>

#include <PerfTimer.hpp>
#include <cryptoException.hpp>
#include <registers.hpp>

class CryptoPoller {

  public:
    static bool isInitialized;

    CryptoPoller(unsigned int timeoutMilliseconds = 100);
    uint32_t execute(uint32_t instruction);

  private:
    unsigned int timeoutMilliseconds;
    static uint32_t currentBitFlip;

    static void writeR0(uint32_t value) { wmem(R0_ADDR, value); }
    static uint32_t readR0() { return rmem(R0_ADDR); }
    static uint32_t readR1() { return rmem(R1_ADDR); }

    uint32_t poll(uint32_t mask, uint32_t endValue);
    void initialize();
};