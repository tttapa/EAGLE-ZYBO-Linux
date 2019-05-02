
#include <BaremetalCommunicationDef.hpp>
#include <SharedMem.hpp>
#include <gtest/gtest.h>

struct BaremetalTestStruct {
  private:
    mutable atomic_flag32 test_lock;

    uint32_t counter = 0;

  public:
    uint32_t ack = 1;

    constexpr static uintptr_t address = SHARED_MEM_START_ADDRESS + 0x1000;

#ifdef BAREMETAL  // Only Baremetal can initialize the shared memory

    /**
     * @brief   Construct the communication struct in shared memory.
     */
    static void init() {
        static_assert(address >= SHARED_MEM_START_ADDRESS);
        static_assert(address <=
                      SHARED_MEM_LAST_ADDRESS - sizeof(BaremetalTestStruct));
        new ((void *) address) BaremetalTestStruct();
    }

#endif

    void increment() volatile {
        ScopedLock lock(test_lock);
        volatile uint32_t tmp = counter;
        usleep(100);
        tmp     = tmp + 1;
        counter = tmp;
    }

    uint32_t getCounter() const volatile {
        ScopedLock lock(test_lock);
        return counter;
    }

#ifdef ZYBO
  private:
    BaremetalTestStruct() { static_assert(sizeof(*this) == 3 * 4); }
#else
    BaremetalTestStruct() = default;
#endif
};

TEST(Comm, SharedMem) {
    BaremetalShared<BaremetalTestStruct> baremetal;

    // Wait for the baremetal application to start
    while (baremetal->ack != 1)
        usleep(1000);
    baremetal->ack = 2;

    // Increment the shared value many times
    for (size_t i = 0; i < 10'000; ++i)
        baremetal->increment();

    // Wait for baremetal to finish
    while (baremetal->ack != 3)
        usleep(1000);

    uint32_t result = baremetal->getCounter();
    ASSERT_EQ(result, 20'000);
};