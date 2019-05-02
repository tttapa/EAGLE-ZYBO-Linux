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
const size_t PAGE_SIZE    = getpagesize();
const uintptr_t PAGE_MASK = ~((uintptr_t) PAGE_SIZE - 1);
#else
#include <memory>
#endif

#ifdef ZYBO
class SharedMemReferenceCounter {
  public:
    SharedMemReferenceCounter() {
        if (count == 0)  // If this is the first instance
            openMem();   // the memory has to be opened
        ++count;
    }
    ~SharedMemReferenceCounter() {
        --count;
        if (count == 0)  // If this was the last instance
            closeMem();  // the memory has to be closed
    }

    int getFileDescriptor() const { return mem_fd; }

  private:
    void openMem() {
        bool cached = false;
        mem_fd      = open(                       //
            "/dev/mem",                      // file
            O_RDWR | (!cached ? O_SYNC : 0)  // oflags
        );
        if (mem_fd < 0) {
            std::ostringstream oss;
            oss << "open(/dev/mem) failed (" << errno << ")";
            std::cerr << ANSIColors::redb << "open(/dev/mem) failed (" << errno
                      << ")" << ANSIColors::reset << std::endl;
            throw std::runtime_error(oss.str());
        }
    }

    void closeMem() { close(mem_fd); }

    static size_t count;
    static int mem_fd;
};
#endif

template <class T>
class BaremetalShared {
  public:
    BaremetalShared() {
#ifdef ZYBO
        // Get the base address of the page, and the offset within the page.
        uintptr_t offset = T::address;
        uintptr_t base   = offset & PAGE_MASK;
        offset &= ~PAGE_MASK;
        assert(offset + sizeof(T) <=  // TODO <= or < ?
               PAGE_SIZE);

        // Map the hardware address of the shared memory region into the virtual
        // address space of the program.
        // TODO: should the offset should be aligned to a page? Can we map just 
        // the size of the struct, or do we have to map the entire page?
        memmap = mmap(                      //
            nullptr,                        // address
            PAGE_SIZE,                      // length
            PROT_READ | PROT_WRITE,         // prot
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
#else
        structdata = std::make_unique<T>();
#endif
    }

#ifdef ZYBO
    ~BaremetalShared() { munmap(memmap, PAGE_SIZE); }
#endif

    BaremetalShared(const BaremetalShared &) = delete;
    BaremetalShared &operator=(const BaremetalShared &) = delete;

#ifdef ZYBO
    volatile T *operator->() { return structdata; }
#else
    volatile T *operator->() { return structdata.get(); }
#endif

  private:
#ifdef ZYBO
    volatile T *structdata;
    void *memmap;
    SharedMemReferenceCounter sharedMem;
#else
    std::unique_ptr<volatile T> structdata;
#endif
};