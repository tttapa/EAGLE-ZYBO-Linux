#include <cryptoPoller.hpp>

bool CryptoPoller::isInitialized = false;

CryptoPoller::CryptoPoller(uint16_t timeoutMilliseconds)
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

void CryptoPoller::reset() { execute(0x00'00'00'00); }

void CryptoPoller::write(uint32_t pos, uint32_t data) {
    pos = pos << 7 * 4;
    data &= 0x00'0F'FF'FF;
    execute(pos | data | 0x00'10'00'00);
}

uint32_t CryptoPoller::read(uint32_t pos) {
    pos = pos << 7 * 4;
    return execute(pos | 0x00'20'00'00);
}

void CryptoPoller::start() { execute(0x00'40'00'00); }

uint32_t CryptoPoller::normalStep(uint32_t data) {
    data &= 0x00'0F'FF'FF;
    return execute(data | 0x00'80'00'00);
}

uint32_t CryptoPoller::xorStep(uint32_t data) {
    data &= 0x00'0F'FF'FF;
    return execute(data | 0x00'90'00'00);
}

uint32_t CryptoPoller::normalStride(uint32_t data) {
    data &= 0x00'0F'FF'FF;
    return execute(data | 0x01'00'00'00);
}

uint32_t CryptoPoller::xorStride(uint32_t data) {
    data &= 0x00'0F'FF'FF;
    return execute(data | 0x01'10'00'00);
}

uint32_t CryptoPoller::hashAbsorb(uint32_t data) {
    data &= 0x00'0F'FF'FF;
    return execute(data | 0x02'00'00'00);
}

uint32_t CryptoPoller::hashSqueeze() { return execute(0x04'00'00'00); }