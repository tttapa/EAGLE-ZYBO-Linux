#include <LogEntry.h>
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
    QR_READ_REQUEST = 1,
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

    void operator=(const Position &p) volatile {
        this->x = p.x;
        this->y = p.y;
    }

    Position() = default;
    Position(float x, float y) : x{x}, y{y} {}
    Position(const volatile Position &pos) : x{pos.x}, y{pos.y} {}
    Position(const Position &pos) : x{pos.x}, y{pos.y} {}
};

class ScopedLock {
  public:
    ScopedLock(volatile std::atomic_flag &lock) : lock{lock} {
        bool locked = true;
        for (size_t i = 0; i < NUM_RETRIES; ++i) {
            locked = lock.test_and_set(std::memory_order_acquire);
            std::cout << "locked = " << locked << std::endl;
            if (locked)
                usleep(WAIT_TIME);
            else
                break;
        }
        if (locked)
            throw std::runtime_error("Timeout: Could not acquire lock");
    }

    ~ScopedLock() {
        lock.clear(std::memory_order_release);
        std::cout << "released" << std::endl;
    }

  private:
    volatile std::atomic_flag &lock;
    constexpr static size_t NUM_RETRIES   = 10;
    constexpr static useconds_t WAIT_TIME = 50;
};

constexpr uintptr_t SHARED_MEM_START_ADDRESS = 0xFFFF0000;
constexpr uintptr_t SHARED_MEM_LAST_ADDRESS  = 0xFFFFFFFF;

#define atomic_flag32 std::atomic_flag __attribute__((aligned(4)))

template <class T>
struct SharedStruct {
#ifdef BAREMETAL  // Only Baremetal can initialize the shared memory
  public:
    static volatile T *init(uintptr_t address) {
        assert(address >= SHARED_MEM_START_ADDRESS);
        assert(address <= SHARED_MEM_LAST_ADDRESS - sizeof(T));
        return new ((void *) address) T();
    }
    static volatile T *init() {
        static_assert(T::address >= SHARED_MEM_START_ADDRESS);
        static_assert(T::address <= SHARED_MEM_LAST_ADDRESS - sizeof(T));
        return new ((void *) T::address) T();
    }
#endif

  protected:
    SharedStruct() = default;
};

struct BaremetalCommStruct : SharedStruct<BaremetalCommStruct> {
  private:
    mutable atomic_flag32 vision_lock = ATOMIC_FLAG_INIT;

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
    mutable atomic_flag32 crypto_lock = ATOMIC_FLAG_INIT;
    Position target;

  public:
    constexpr static uintptr_t address = SHARED_MEM_START_ADDRESS + 0x1000;

    BaremetalCommStruct() { static_assert(sizeof(*this) == 11 * 4); }

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
};

struct AccessControlledLogEntry : SharedStruct<AccessControlledLogEntry> {
  private:
    // Initialize to true, so it triggers a log write
    mutable bool32 doneReading = true;
    LogEntry logEntry;

  public:
#ifdef BAREMETAL
    void write(const LogEntry &logEntry) const volatile {
        if (!isDoneReading())
            throw std::runtime_error("Error: illegal LogEntry write");
        // The logEntry shouldn't change when doneReading == true, so we can
        // safely cast away the volatile qualifier.
        const_cast<LogEntry &>(this->logEntry) = logEntry;
        doneReading                            = false;
    }
#else
    LogEntry read() const volatile {
        if (!isDoneWriting())
            throw std::runtime_error("Error: illegal LogEntry read");
        // The logEntry shouldn't change when doneReading == false, so we can
        // safely cast away the volatile qualifier.
        LogEntry tmp = const_cast<const LogEntry &>(logEntry);
        doneReading  = true;
        return tmp;
    }
#endif

    bool isDoneReading() const volatile { return doneReading; }
    bool isDoneWriting() const volatile { return !doneReading; }

    constexpr static uintptr_t address = SHARED_MEM_START_ADDRESS + 0x3000;
};

// #undef atomic_flag32