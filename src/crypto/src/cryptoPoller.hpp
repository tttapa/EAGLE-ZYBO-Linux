#pragma once

#include <cryptoException.hpp>
#include <cstdint>
#include <registers.hpp>

class CryptoPoller {

  public:
    CryptoPoller(uint16_t timeoutMilliseconds = 100);

    void reset();
    void write(uint32_t pos, uint32_t data);
    uint32_t read(uint32_t pos);
    void start();
    uint32_t normalStep(uint32_t data);
    uint32_t xorStep(uint32_t data);
    uint32_t normalStride(uint32_t data);
    uint32_t xorStride(uint32_t data);
    uint32_t hashAbsorb(uint32_t data);
    uint32_t hashSqueeze();

  private:
    static bool isInitialized;
    static uint32_t currentBitFlip;
    uint16_t timeoutMilliseconds;

    static void writeR0(uint32_t value);
    static uint32_t readR0();
    static uint32_t readR1();

    uint32_t poll(uint32_t mask, uint32_t previousValue);
    void initialize();

    uint32_t execute(uint32_t instruction);

    const uint32_t fullMask = 0x00'0F'FF'FF;
};