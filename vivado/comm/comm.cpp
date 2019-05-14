#ifdef ZYBO

#include <BaremetalCommunicationDef.hpp>
#include <iostream>

struct TestStruct : SharedStruct<TestStruct> {
    uint32_t l2b = 0;
    uint32_t b2l = 0;

    constexpr static uintptr_t address = SHARED_MEM_START_ADDRESS + 0xF000;
};

#ifndef BAREMETAL

#include <SharedMem.hpp>
#include <gtest/gtest.h>

/**
 * Wait for Baremetal to be initialized, then write a value to the
 * Linux2Baremetal (l2b) field, wait, and check that Baremetal2Linux (b2l)
 * has been updated by Baremetal to the value of l2b + 1.
 */
TEST(Comm, SharedMem) {
    SharedMemory<TestStruct> baremetal;
    while (!baremetal->isInitialized())
        usleep(10'000);
    uint32_t test  = 0x12345678;
    baremetal->l2b = test;
    usleep(100'000);
    ASSERT_EQ(baremetal->b2l, ~test);
    baremetal->l2b = 0;
};

#else

#include "eagle_setup_ipc.hpp"

int main() {
    eagle_setup_ipc();
    auto testcomm   = TestStruct::init();
    auto visioncomm = VisionCommStruct::init();
    auto qrcomm     = QRCommStruct::init();
    auto logger     = AccessControlledLogEntry::init();
    std::cout << "C++ Baremetal Started" << std::endl;
    while (1) {
        uint32_t l2b = testcomm->l2b;
        if (l2b != 0)
            testcomm->b2l = ~l2b;
        if (visioncomm->isDoneWriting()) {
            VisionData visiondata = visioncomm->read();
            std::cout << "Vision: " << visiondata.position << ", "
                      << visiondata.yawAngle << std::endl;
        }
        if (logger->isDoneReading()) {
            LogEntry logEntry;
            // logEntry.setSomething(...);
            logger->write(logEntry);
        }
    }
}

#endif  // BAREMETAL

#endif  // ZYBO