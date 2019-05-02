#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread> // TODO: I don't think this works on Baremetal

using bool32 = uint32_t;

enum class QRFSMState : int32_t {
    IDLE            = 0,
    QR_READ         = 1,
    QR_READING_BUSY = 2,
    CRYPTO_BUSY     = 3,
    NEW_TARGET      = 4,
    LAND            = 5,
    QR_UNKNOWN      = 6,
    ERROR           = 7,
};

enum class FlightMode : int32_t {
    MANUAL        = 0,
    ALTITUDE_HOLD = 1,
    AUTONOMOUS    = 2,
};

class ScopedLock {
  public:
    ScopedLock(volatile std::atomic_flag &lock) : lock{lock} {
        bool failed = true;
        for (size_t i = 0; i < NUM_RETRIES; ++i) {
            if (failed = lock.test_and_set(); failed)
                // TODO: can we sleep like this on baremetal?
                std::this_thread::sleep_for(WAIT_TIME);
            else
                break;
        }
        if (failed)
            throw std::runtime_error("Timeout: Could not acquire lock");
    }
    ScopedLock(
        volatile decltype(std::atomic_flag::__atomic_flag_base::_M_i) &lock)
        : ScopedLock{reinterpret_cast<volatile std::atomic_flag &>(lock)} {}

    ~ScopedLock() { lock.clear(); }

  private:
    volatile std::atomic_flag &lock;
    constexpr static size_t NUM_RETRIES = 10;
    constexpr static std::chrono::microseconds WAIT_TIME =
        std::chrono::microseconds(500);
};

constexpr uintptr_t SHARED_MEM_START_ADDRESS = 0xFFFF0000;
constexpr uintptr_t SHARED_MEM_LAST_ADDRESS  = 0xFFFFFFFF;

#define atomic_flag32 std::atomic_flag __attribute__((aligned(4)))

/**
 * @brief   Struct for communication between the Linux core and the Baremetal 
 *          core.
 */
struct BaremetalCommStruct {
  private:
    atomic_flag32 vision_lock;

  public:
    FlightMode mode;
    bool32 init;
    bool32 inductive;

  private:
    float positionX, positionY;
    float yawAngle;

  public:
    QRFSMState qrState;

  private:
    atomic_flag32 crypto_lock;
    float targetX, targetY;

  public:
    constexpr static uintptr_t address = SHARED_MEM_START_ADDRESS + 0x1000;

#ifdef BAREMETAL  // Only Baremetal can initialize the shared memory

    /**
     * @brief   Construct the communication struct in shared memory.
     */
    void init() {
        static_assert(address >= SHARED_MEM_START_ADDRESS);
        static_assert(address <= SHARED_MEM_LAST_ADDRESS - sizeof(*this));
        new ((void *) address) BaremetalCommStruct();
    }

#endif

    void setPosition(float x, float y) volatile {
        ScopedLock lock(vision_lock);
        this->positionX = x;
        this->positionY = y;
    }

    void setTargetPosition(float x, float y) volatile {
        ScopedLock lock(crypto_lock);
        this->targetX = x;
        this->targetY = y;
    }

#ifdef ZYBO
  private:
    BaremetalCommStruct() { static_assert(sizeof(*this) == 11 * 4); }
#else
    BaremetalCommStruct() = default;
#endif
};

#undef atomic_flag32