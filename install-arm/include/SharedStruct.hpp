#pragma once

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>

/// A 32-bit boolean type to ensure alignment and packing of the shared structs.
using bool32 = uint32_t;

/// The start address of the shared memory region in the On-Chip Memory (OCM).
constexpr uintptr_t SHARED_MEM_START_ADDRESS = 0xFFFF0000;
/// The last valid address of the shared memory region in th On-Chip Memory.
constexpr uintptr_t SHARED_MEM_LAST_ADDRESS  = 0xFFFFFFFF;

/**
 * @brief   A base class for structs that are shared between Linux and 
 *          Baremetal. It checks initialization.
 * 
 * @tparam  T 
 *          The type of data to share.
 */
template <class T>
struct SharedStruct {
#ifdef BAREMETAL  // Only Baremetal can initialize the shared memory
  public:
    /// Initialize the data structure at the given address. (Baremetal only)
    static volatile T *init(uintptr_t address) {
        assert(address >= SHARED_MEM_START_ADDRESS);
        assert(address <= SHARED_MEM_LAST_ADDRESS - sizeof(T));
        return new ((void *) address) T();
    }
    /// Initialize the data structure at its default address. (Baremetal only)
    static volatile T *init() {
        static_assert(T::address >= SHARED_MEM_START_ADDRESS, 
                      "Error: address not inside of shared memory region");
        static_assert(T::address <= SHARED_MEM_LAST_ADDRESS - sizeof(T),
                      "Error: address not inside of shared memory region");
        return new ((void *) T::address) T();
    }
#endif

    /// Check if the data structure has been initialized by Baremetal.
    bool isInitialized() const volatile {
        return initialized == INIT_MAGIC_NUM;  // TODO: do we need a handshake?
    }
    /// Check if the data structure has been initialized by Baremetal, 
    /// and throw an exception if this is not the case.
    void checkInitialized() const volatile {
        if (!isInitialized())
            throw std::runtime_error("Error: Baremetal not yet initialized");
    }

    /**
     * @brief   Get the next address right after this data structure that can be
     *          used by the next data structure.
     * @return  The address of this struct plus its size.
     */
    static constexpr uintptr_t nextFreeAddress() {
        return T::address + sizeof(T);
    }

  protected:
    SharedStruct() = default;

  private:
    uint32_t initialized                     = INIT_MAGIC_NUM;
    constexpr static uint32_t INIT_MAGIC_NUM = 0xDEADBEEF ^ sizeof(T);
};

/** 
 * @brief   Option for one-way communication structs that send data from 
 *          Baremetal to Linux.
 */
struct Baremetal2Linux {
    constexpr static const char *writer = "Baremetal";
    constexpr static const char *reader = "Linux";
};
/** 
 * @brief   Option for one-way communication structs that send data from 
 *          Linux to Baremetal.
 */
struct Linux2Baremetal {
    constexpr static const char *writer = "Linux";
    constexpr static const char *reader = "Baremetal";
};

/**
 * @brief   A struct that adds access control to a communication struct.
 *          It ensures that data can only flow in the specified direction,
 *          and that no reads or writes occur unless the other party is ready.
 * 
 * @tparam  T
 *          The type of the data to share. 
 * @tparam  Dir 
 *          The direction of the data flow.
 * @tparam  Addr 
 *          The default address in shared memory to use.
 */
template <class T, class Dir, uintptr_t Addr>
struct AccessControlledSharedStruct
    : SharedStruct<AccessControlledSharedStruct<T, Dir, Addr>> {
  private:
    // Initialize to true, so it triggers a write when updated.
    mutable bool32 doneReading = true;
    T data = {};

  public:
#ifdef BAREMETAL
    using WDir = Baremetal2Linux;
#else
    using WDir = Linux2Baremetal;
#endif

    /**
     * @brief   Write data to the communication struct.
     * 
     * @param   data
     *          The data to send. 
     * @throws  std::runtime_error
     *          If the data has not yet been initialized by Baremetal.
     * @throws  std::runtime_error
     *          If the other party is not yet done reading.
     */
    template <class R = void>
    typename std::enable_if_t<std::is_same_v<Dir, WDir>, R>  //
    // void  //
    write(const T &data) volatile {
        this->checkInitialized();
        if (!isDoneReading())
            throw std::runtime_error(std::string("Error: illegal write: ") +
                                     Dir::reader + " is not yet done reading");
        // The data shouldn't change when doneReading == true, so we can
        // safely cast away the volatile qualifier.
        const_cast<T &>(this->data) = data;
        doneReading                 = false;
    }

    /**
     * @brief   Read data from the communication struct.
     * 
     * @return  The data from shared memory. 
     * @throws  std::runtime_error
     *          If the data has not yet been initialized by Baremetal.
     * @throws  std::runtime_error
     *          If the other party is not yet done writing.
     */
    template <class R = T>
    typename std::enable_if_t<!std::is_same_v<Dir, WDir>, R>  //
    // T  //
    read() const volatile {
        this->checkInitialized();
        if (!isDoneWriting())
            throw std::runtime_error(std::string("Error: illegal read: ") +
                                     Dir::writer + " is not yet done writing");
        // The data shouldn't change when doneReading == false, so we can
        // safely cast away the volatile qualifier.
        T tmp       = const_cast<const T &>(data);
        doneReading = true;
        return tmp;
    }

    /// Check if the other party is done reading
    bool isDoneReading() const volatile { return doneReading; }
    /// Check if the other party is done writing
    bool isDoneWriting() const volatile { return !doneReading; }

    constexpr static uintptr_t address = Addr;
};