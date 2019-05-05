#pragma once

#include <SharedMem.hpp>
#include <cryptoException.hpp>
#include <cstdint>

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
    struct FPGARegisters {
        uint32_t R0 = 0;  // Default initialization just keeps the compiler
        const uint32_t R1 = 0;  // happy, it isn't used.

        constexpr static uintptr_t address = 0x43C70000;
    };
    SharedMemory<FPGARegisters> fpga;

    static bool isInitialized;
    static uint32_t currentBitFlip;
    uint16_t timeoutMilliseconds;

    void writeR0(uint32_t value);
    uint32_t readR0() const;
    uint32_t readR1() const;

    uint32_t poll(uint32_t mask, uint32_t previousValue);
    void initialize();

    uint32_t execute(uint32_t instruction);

    const uint32_t fullMask = 0x00'0F'FF'FF;
};