#include <atomic>
#include <cmath>  // NAN
#include <cstdint>
#include <iostream>
#include <stdexcept>

#ifdef BAREMETAL
#include <ps7_cortexa9_1/include/sleep.h>
#else
#include <unistd.h>
#endif

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

struct Position {
    float x = NAN, y = NAN;
    explicit operator bool() const volatile {
        return !(std::isnan(x) || std::isnan(y));
    }
#if 0
	volatile Position& operator = (const Position& p) volatile {
		this->x = p.x;
		this->y = p.y;
		return *this;
	}
#else
    void operator=(const Position &p) volatile {
        this->x = p.x;
        this->y = p.y;
    }
#endif
    Position() = default;
    Position(float x, float y) : x{x}, y{y} {}
    Position(const volatile Position &pos) : x{pos.x}, y{pos.y} {}
    Position(const Position &pos) : x{pos.x}, y{pos.y} {}
};

class ScopedLock {
  public:
    ScopedLock(volatile std::atomic_flag &lock) : lock{lock} {
        bool failed = true;
        for (size_t i = 0; i < NUM_RETRIES; ++i) {
            if (failed = lock.test_and_set(); failed)
                usleep(WAIT_TIME);
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
    constexpr static size_t NUM_RETRIES   = 10;
    constexpr static useconds_t WAIT_TIME = 500;
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
    mutable atomic_flag32 vision_lock;

  public:
    FlightMode mode    = FlightMode::MANUAL;
    bool32 initialised = true;  // TODO: do we need a handshake?
    bool32 inductive   = false;

  private:
    Position position;
    float yawAngle;

  public:
    QRFSMState qrState;

  private:
    mutable atomic_flag32 crypto_lock;
    Position target;

  public:
    constexpr static uintptr_t address = SHARED_MEM_START_ADDRESS + 0x1000;

#ifdef BAREMETAL  // Only Baremetal can initialize the shared memory

    /**
     * @brief   Construct the communication struct in shared memory.
     */
    static void init() {
        static_assert(address >= SHARED_MEM_START_ADDRESS);
        static_assert(address <=
                      SHARED_MEM_LAST_ADDRESS - sizeof(BaremetalCommStruct));
        new ((void *) address) BaremetalCommStruct();
    }

#endif

    void setVisionPosition(Position pos) volatile {
        ScopedLock lock(vision_lock);
        this->position = pos;
    }
    void setVisionPosition(float x, float y) volatile {
        setVisionPosition({x, y});
    }

    void setTargetPosition(Position target) volatile {
        ScopedLock lock(crypto_lock);
        this->target = target;
    }
    void setTargetPosition(float x, float y) volatile {
        setTargetPosition({x, y});
    }

    Position getPosition() const volatile {
        ScopedLock lock(vision_lock);
        return this->position;
    }

#ifdef ZYBO
  private:
    BaremetalCommStruct() { static_assert(sizeof(*this) == 11 * 4); }
#else
    BaremetalCommStruct() = default;
#endif
};
