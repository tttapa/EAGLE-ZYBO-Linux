#pragma once

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>

using bool32 = uint32_t;

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

    static constexpr uintptr_t nextFreeAddress() {
        return T::address + sizeof(T);
    }

  protected:
    SharedStruct() = default;

  private:
    uint32_t initialized                     = INIT_MAGIC_NUM;
    constexpr static uint32_t INIT_MAGIC_NUM = 0xDEADBEEF ^ sizeof(T);
};

struct Baremetal2Linux {
    constexpr static const char *writer = "Baremetal";
    constexpr static const char *reader = "Linux";
};
struct Linux2Baremetal {
    constexpr static const char *writer = "Linux";
    constexpr static const char *reader = "Baremetal";
};

template <class T, class Dir, uintptr_t Addr>
struct AccessControlledSharedStruct
    : SharedStruct<AccessControlledSharedStruct<T, Dir, Addr>> {
  private:
    // Initialize to true, so it triggers a write when updated.
    mutable bool32 doneReading = true;
    T data;

  public:
#ifdef BAREMETAL
    using WDir = Baremetal2Linux;
#else
    using WDir = Linux2Baremetal;
#endif

    template <class R = void>
    typename std::enable_if_t<std::is_same_v<Dir, WDir>, R>  //
    // void  //
    write(const T &data) volatile {
        if (!isDoneReading())
            throw std::runtime_error(std::string("Error: illegal write: ") +
                                     Dir::reader + " is not yet done reading");
        // The data shouldn't change when doneReading == true, so we can
        // safely cast away the volatile qualifier.
        const_cast<T &>(this->data) = data;
        doneReading                 = false;
    }

    template <class R = T>
    typename std::enable_if_t<!std::is_same_v<Dir, WDir>, R>  //
    // T  //
    read() const volatile {
        if (!isDoneWriting())
            throw std::runtime_error(std::string("Error: illegal read: ") +
                                     Dir::writer + " is not yet done writing");
        // The data shouldn't change when doneReading == false, so we can
        // safely cast away the volatile qualifier.
        T tmp       = const_cast<const T &>(data);
        doneReading = true;
        return tmp;
    }

    bool isDoneReading() const volatile { return doneReading; }
    bool isDoneWriting() const volatile { return !doneReading; }

    constexpr static uintptr_t address = Addr;
};