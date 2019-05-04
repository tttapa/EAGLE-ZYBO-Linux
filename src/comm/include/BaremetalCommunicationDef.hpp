#include <LogEntry.h>
#include <cassert>
#include <cmath>  // NAN
#include <cstdint>
#include <iostream>
#include <stdexcept>

using bool32 = uint32_t;

enum class QRFSMState : int32_t {
    IDLE            = 0,
    QR_READ_REQUEST = 1,
    QR_READING_BUSY = 2,
    NEW_TARGET      = 3,
    LAND            = 4,
    QR_UNKNOWN      = 5,
    ERROR           = -1,
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

constexpr uintptr_t SHARED_MEM_START_ADDRESS = 0xFFFF0000;
constexpr uintptr_t SHARED_MEM_LAST_ADDRESS  = 0xFFFFFFFF;

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

    bool isInitialized() const volatile {
        return initialized == INIT_MAGIC_NUM;  // TODO: do we need a handshake?
    }
    void checkInitialized() const volatile {
        if (!isInitialized())
            throw std::runtime_error("Error: Baremetal not yet initialized");
    }

  protected:
    SharedStruct() = default;

  private:
    uint32_t initialized                     = INIT_MAGIC_NUM;
    constexpr static uint32_t INIT_MAGIC_NUM = 0xDEADBEEF;
};

struct BaremetalCommStruct : SharedStruct<BaremetalCommStruct> {
  public:
    FlightMode mode  = FlightMode::MANUAL;
    bool32 inductive = false;

  private:
    enum class VisionState : bool32 {
        BAREMETAL_READING_DONE,
        VISION_WRITING_DONE,
    } mutable visionState = VisionState::BAREMETAL_READING_DONE;

  private:
    Position position;
    float yawAngle;

    mutable QRFSMState qrState;
    Position target;

  public:
    constexpr static uintptr_t address = SHARED_MEM_START_ADDRESS + 0x1000;

    BaremetalCommStruct() { static_assert(sizeof(*this) == 10 * 4); }

    VisionState getVisionState() const volatile { return visionState; }
    QRFSMState getQRState() const volatile { return qrState; }

#ifndef BAREMETAL
    void setVisionPosition(Position pos) volatile {
        checkInitialized();
        if (getVisionState() != VisionState::BAREMETAL_READING_DONE)
            throw std::runtime_error("Error: illegal setVisionPosition call: "
                                     "Baremetal not yet done reading");
        this->position = pos;
        visionState    = VisionState::VISION_WRITING_DONE;
    }
    void setVisionPosition(float x, float y) volatile {
        setVisionPosition({x, y});
    }

    void setTargetPosition(Position target) volatile {
        checkInitialized();
        if (getQRState() == QRFSMState::NEW_TARGET)
            throw std::runtime_error("Error: illegal setTargetPosition call: "
                                     "Baremetal not yet done reading");
        this->target = target;
        qrState      = QRFSMState::NEW_TARGET;
    }
    void setTargetPosition(float x, float y) volatile {
        setTargetPosition({x, y});
    }

    void setQRStateBusy() volatile {
        if (getQRState() != QRFSMState::QR_READ_REQUEST)
            throw std::runtime_error("Error: illegal QR state transition: "
                                     "Only QR_READ_REQUEST → QR_READING_BUSY "
                                     "is allowed");
        qrState = QRFSMState::QR_READING_BUSY;
    }
    void setQRStateError() volatile { qrState = QRFSMState::ERROR; }
    void setQRStateUnkown() volatile { qrState = QRFSMState::QR_UNKNOWN; }
    void setQRStateLand() volatile { qrState = QRFSMState::LAND; }
#else
    Position getVisionPosition() const volatile {
        if (getVisionState() != VisionState::VISION_WRITING_DONE)
            throw std::runtime_error("Error: illegal getVisionPosition call: "
                                     "Vision not yet done writing");
        Position position = this->position;
        visionState       = VisionState::BAREMETAL_READING_DONE;
        return position;
    }
    Position getTargetPosition() const volatile {
        if (getQRState() != QRFSMState::NEW_TARGET)
            throw std::runtime_error("Error: illegal getTargetPosition call: "
                                     "No new target available");
        Position target = this->target;
        qrState         = QRFSMState::IDLE;
        return target;
    }
#endif
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