#ifdef ZYBO

#include <BaremetalCommunicationDef.hpp>
#include <SharedMem.hpp>
#include <gtest/gtest.h>

struct BaremetalTestStruct {
  private:
    mutable atomic_flag32 test_lock = ATOMIC_FLAG_INIT;

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
        {
            std::cout << "lock" << std::endl;
            // ScopedLock lock(test_lock);
            std::cout << "locked" << std::endl;
            volatile uint32_t tmp = counter;
            usleep(40);
            tmp     = tmp + 1;
            counter = tmp;
            std::cout << "+";
        }
        usleep(10);
    }

    void clear() volatile { test_lock.clear(std::memory_order_release); }

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

#include <wrapper.h>

struct TestStruct {
    uint32_t l2b = 0;
    uint32_t b2l = 0;

    constexpr static uintptr_t address = SHARED_MEM_START_ADDRESS + 0x2000;

#ifdef BAREMETAL  // Only Baremetal can initialize the shared memory

    /**
     * @brief   Construct the communication struct in shared memory.
     */
    static volatile TestStruct *init() {
        static_assert(address >= SHARED_MEM_START_ADDRESS);
        static_assert(address <= SHARED_MEM_LAST_ADDRESS - sizeof(TestStruct));
        return new ((void *) address) TestStruct();
    }

#endif

  private:
    TestStruct() = default;
};

TEST(Comm, SharedMem) {
#if 0
    BaremetalShared<BaremetalTestStruct> baremetal;
    baremetal->clear();
    baremetal->ack = 1;

    // Wait for the baremetal application to start
    std::cout << "Waiting for Baremetal" << std::endl;
    while (baremetal->ack != 2)
        ;
    std::cout << "Go\n";

    // Increment the shared value many times
    for (size_t i = 0; i < 1; ++i)
        baremetal->increment();

    std::cout << std::endl << "Waiting for Baremetal to finish" << std::endl;
    // Wait for baremetal to finish
    while (baremetal->ack != 3)
        usleep(100);

    uint32_t result = baremetal->getCounter();
    ASSERT_EQ(result, 2);
#else
    BaremetalShared<TestStruct> baremetal;
    while (1) {
        baremetal->l2b++;
        std::cout << "b2l = " << baremetal->b2l << ", l2b = " << baremetal->l2b
                  << std::endl;
        usleep(1'000'000);
    }
#endif
};

#ifdef BAREMETAL

// ================================================================== //

#include "xil_mmu.h"
#include "xil_cache.h"
#include "xscugic.h"

void eagle_setup_ipc(void);
void eagle_setup_clk(void);
void eagle_DCacheFlush(void);
void eagle_SetTlbAttributes(u32 addr, u32 attrib);

extern "C" void Xil_L1DCacheFlush(void);

extern u32 MMUTable;

void eagle_setup_ipc(void){
	eagle_SetTlbAttributes(0xFFFF0000, 0x04de2);
}

void eagle_DCacheFlush(void) {
	Xil_L1DCacheFlush();
	//Xil_L2CacheFlush();
}

void eagle_SetTlbAttributes(u32 addr, u32 attrib) {
	u32 *ptr;
	u32 section;

	mtcp(XREG_CP15_INVAL_UTLB_UNLOCKED, 0);
	dsb();

	mtcp(XREG_CP15_INVAL_BRANCH_ARRAY, 0);
	dsb();
	eagle_DCacheFlush();

	section = addr / 0x100000;
	ptr = &MMUTable + section;
	*ptr = (addr & 0xFFF00000) | attrib;
	dsb();
}

// ================================================================== //

#ifdef BAREMETAL
#include <ps7_cortexa9_1/include/sleep.h>
#else
#include <unistd.h>
#endif
#include <iostream>
#include <new>

constexpr uintptr_t SHARED_MEM_START_ADDRESS = 0xFFFF0000;
constexpr uintptr_t SHARED_MEM_LAST_ADDRESS  = 0xFFFFFFFF;

struct TestStruct {
    uint32_t l2b = 0;
    uint32_t b2l = 0;

    constexpr static uintptr_t address = SHARED_MEM_START_ADDRESS + 0x2000;

#ifdef BAREMETAL  // Only Baremetal can initialize the shared memory

    /**
     * @brief   Construct the communication struct in shared memory.
     */
    static volatile TestStruct *init() {
        static_assert(address >= SHARED_MEM_START_ADDRESS);
        static_assert(address <= SHARED_MEM_LAST_ADDRESS - sizeof(TestStruct));
        return new ((void *) address) TestStruct();
    }

#endif

  private:
    TestStruct() = default;
};

// ================================================================== //

int main() {
	eagle_setup_ipc();
    volatile TestStruct *baremetal = TestStruct::init();
    while(1) {
        baremetal->b2l++;
        std::cout << "b2l = " << baremetal->b2l << ", l2b = " << baremetal->l2b << std::endl;
        usleep(1'000'000);
    }
}

#endif

#endif