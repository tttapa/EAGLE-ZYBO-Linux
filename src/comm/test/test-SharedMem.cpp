#ifdef ZYBO

#include <BaremetalCommunicationDef.hpp>
#include <SharedMem.hpp>

struct TestStruct : SharedStruct<TestStruct> {
    bool32 initialized;
    uint32_t l2b = 0;
    uint32_t b2l = 0;

    constexpr static uintptr_t address = SHARED_MEM_START_ADDRESS + 0xF000;

  private:
    TestStruct() = default;
};

#ifndef BAREMETAL

#include <gtest/gtest.h>

/**
 * Wait for Baremetal to be initialized, then write a value to the 
 * Linux2Baremetal (l2b) field, wait, and check that Baremetal2Linux (b2l)
 * has been updated by Baremetal to the value of l2b + 1.
 */
TEST(Comm, SharedMem) {
    BaremetalShared<TestStruct> baremetal;
    while (!baremetal->isInitialized())
        usleep(10'000);
    uint32_t test  = 0x12345678;
    baremetal->l2b = test;
    usleep(100'000);
    ASSERT_EQ(baremetal->b2l, ~test);
};

#else

int main() {
    auto comm = TestStruct::init();
    while (comm->l2b == 0)
        ;
    comm->b2l = ~comm->l2b;
}

#endif  // BAREMETAL

#endif  // ZYBO