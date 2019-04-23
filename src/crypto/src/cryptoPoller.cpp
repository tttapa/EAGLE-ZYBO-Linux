#include <cryptoPoller.hpp>

bool CryptoPoller::isInitialized = false;

CryptoPoller::CryptoPoller(unsigned int timeoutMilliseconds)
    : timeoutMilliseconds(timeoutMilliseconds) {
    if (!isInitialized) {
        initialize();
        isInitialized = true;
    }
}

uint32_t CryptoPoller::execute(uint32_t instruction) {
    writeR0(instruction);
    return poll(0x80'00'00'00, currentBitFlip);
}

uint32_t CryptoPoller::poll(uint32_t mask, uint32_t previousValue) {
    PerfTimer timer;
    bool instructionDone = false;
    uint32_t currentR1;
    while (timer.getDuration<std::chrono::milliseconds>() <=
           timeoutMilliseconds) {
        for (int i = 0; i < 10'000; i++) {
            currentR1 = readR1();
            if ((currentR1 & mask) != previousValue) {
                instructionDone = true;
                break;
            }
        }

        if (instructionDone)
            break;
    }

    if (!instructionDone)
        throw CryptoException(CryptoException::TIMEOUT_EXCEPTION);

    currentBitFlip = currentR1 & 0x80'00'00'00;
    return currentR1;
}

void CryptoPoller::initialize() {
    if (readR1() & 0x00'00'00'01) {
        writeR0(0x10'10'00'00);
        poll(0x00'00'00'01, 0x00'00'00'01);
    } else {
        writeR0(0x10'10'00'01);
        poll(0x00'00'00'01, 0x00'00'00'00);
    }

    execute(0x00'00'00'00);
}