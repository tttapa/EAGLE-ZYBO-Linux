#pragma once

#include <ANSIColors.hpp>
#include <iomanip>
#include <iostream>

#ifdef ZYBO
#include <cassert>
#include <cerrno>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const size_t PAGE_SIZE      = getpagesize();
const uintptr_t PAGE_MASK   = ~((uintptr_t) PAGE_SIZE - 1);
const uintptr_t OFFSET_MASK = (uintptr_t) PAGE_SIZE - 1;

#else
#include <memory>
#endif

#ifdef ZYBO
/**
 * @brief   This class keeps track of how many objects are using the /dev/mem
 *          file. It's not strictly necessary, since we could just close it 
 *          after `mmap`ing the memory, but I had already written this class
 *          when I found that out, and it saves some `open` and `close` system
 *          calls, so I just kept it.
 */
class SharedMemReferenceCounter {
  public:
    /**
     * @brief   Open /dev/mem if there are no active instances.
     */
    SharedMemReferenceCounter() {
        if (count == 0)  // If this is the first instance
            openMem();   // the memory has to be opened
        ++count;
    }
    /**
     * @brief   Close /dev/mem if this is the last active instance.
     */
    ~SharedMemReferenceCounter() {
        --count;
        if (count == 0)  // If this was the last instance
            closeMem();  // the memory has to be closed
    }
    // Delete the copy constructor and copy assignment
    SharedMemReferenceCounter(const SharedMemReferenceCounter &) = delete;
    SharedMemReferenceCounter &
    operator=(const SharedMemReferenceCounter &) = delete;
    // Keep the move constructor and assignment
    SharedMemReferenceCounter(SharedMemReferenceCounter &&) = default;
    SharedMemReferenceCounter &
    operator=(SharedMemReferenceCounter &&) = default;

    /**
     * @brief Get a file descriptor to /dev/mem.
     */
    int getFileDescriptor() const { return mem_fd; }

  private:
    void openMem() {
        mem_fd = open(       //
            "/dev/mem",      // file path
            O_RDWR | O_SYNC  // flags
        );
        if (mem_fd < 0) {
            std::cerr << ANSIColors::redb << "open(/dev/mem) failed (" << errno
                      << ")" << ANSIColors::reset << std::endl;
            std::ostringstream oss;
            oss << "open(/dev/mem) failed (" << errno << ")";
            throw std::runtime_error(oss.str());
        }
    }

    void closeMem() { close(mem_fd); }

    static size_t count;
    static int mem_fd;
};
#endif

/**
 * @brief   A class that manages a mapping to an address in shared memory.
 *          It maps the physical address to an address in virtual memory, and
 *          manages this pointer, unmapping it when the object goes out of 
 *          scope.
 * @tparam  T
 *          The type of the shared object to manage.
 */
template <class T>
class SharedMemory {
  public:
    /**
     * @brief   Construct a new SharedMemory object. Uses `mmap` to map the page
     *          containing the address.
     * 
     * @param   address
     *          The physical address to map. The object should not cross any 
     *          page boundaries.
     */
    SharedMemory(uintptr_t address) {
#ifdef ZYBO
        /*std::cout << __PRETTY_FUNCTION__ << ": " << std::hex << std::showbase
                  << (uintptr_t) address << std::dec << std::noshowbase
                  << std::endl;*/
        // Get the base address of the page, and the offset within the page.
        uintptr_t base   = address & PAGE_MASK;
        uintptr_t offset = address & OFFSET_MASK;
        // Make sure we don't cross the page boundary
        assert(offset + sizeof(T) <= PAGE_SIZE);

        /*std::cout << std::hex << std::showbase << "address = " << address
                  << ", base = " << base << ", offset = " << offset
                  << ", PAGE_SIZE = " << PAGE_SIZE
                  << ", PAGE_MASK = " << PAGE_MASK << std::dec
                  << std::noshowbase << std::endl;*/

        // Map the hardware address of the shared memory region into the virtual
        // address space of the program.
        // Offset should be aligned to a page, and size should be a multiple of
        // the page size.
        memmap = mmap(                      //
            nullptr,                        // address
            PAGE_SIZE,                      // length
            PROT_READ | PROT_WRITE,         // protection
            MAP_SHARED,                     // flags
            sharedMem.getFileDescriptor(),  // file descriptor
            base                            // offset
        );
        if (memmap == MAP_FAILED) {
            std::ostringstream oss;
            oss << "mmap(" << std::hex << std::showbase << base << std::dec
                << std::noshowbase << ") failed (" << errno << ")";
            std::cerr << ANSIColors::redb << "mmap(" << std::hex
                      << std::showbase << base << std::dec << std::noshowbase
                      << ") failed (" << errno << ")" << ANSIColors::reset
                      << std::endl;
            throw std::runtime_error(oss.str());
        }
        uintptr_t memmapp = reinterpret_cast<uintptr_t>(memmap);
        structdata        = reinterpret_cast<volatile T *>(memmapp + offset);

        /*std::cout << std::hex << std::showbase << "memmap = " << memmap
                  << std::endl;
        for (size_t i = 0; i < sizeof(T); ++i) {
            int data = reinterpret_cast<volatile uint8_t *>(structdata)[i];
            std::cout << data << " ";
        }
        std::cout << std::dec << std::noshowbase << std::endl;*/
#else
        (void) address;
        structdata = std::make_unique<T>();
#endif
    }
    /**
     * @brief   Construct a new SharedMemory object. Uses `mmap` to map the page
     *          containing the address. It uses the default address of the type
     *          `T`: `T::address`.
     */
    SharedMemory() : SharedMemory{T::address} {}

#ifdef ZYBO
    /**
     * @brief   Destroy the SharedMemory object. Uses `munmap`.
     */
    ~SharedMemory() {
        if (memmap != nullptr) {
            /*std::cout << __PRETTY_FUNCTION__ << ": " << std::hex
                      << std::showbase << (uintptr_t) memmap << std::dec
                      << std::noshowbase << std::endl;*/
            munmap(memmap, PAGE_SIZE);
        }
    }
#endif

    SharedMemory(const SharedMemory &) = delete;
    SharedMemory &operator=(const SharedMemory &) = delete;

    SharedMemory(SharedMemory &&other) { *this = std::move(other); }
    SharedMemory &operator=(SharedMemory &&other) {
        std::swap(this->structdata, other.structdata);
        std::swap(this->memmap, other.memmap);
        return *this;
    }

    volatile T *ptr() { return structdata; }
    const volatile T *ptr() const { return structdata; }

#ifdef ZYBO
    /**
     * @brief   Proxy for the managed shared object.
     */
    volatile T *operator->() { return structdata; }

    /**
     * @brief   Const proxy for the managed shared object.
     */
    const volatile T *operator->() const { return structdata; }
#else
    volatile T *operator->() { return structdata.get(); }
    const volatile T *operator->() const { return structdata.get(); }
#endif

  private:
    void *memmap = nullptr;
#ifdef ZYBO
    SharedMemReferenceCounter sharedMem = {};
    volatile T *structdata              = nullptr;
#else
    std::unique_ptr<volatile T> structdata = {};
#endif
};