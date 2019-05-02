#pragma once

#include <BaremetalCommunicationDef.hpp>

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

template <class T>
class BaremetalShared {
  public:
    BaremetalShared() {
#ifdef ZYBO
        bool cached = false;

        mem_fd = open(                       //
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

        uintptr_t offset = BAREMETAL_COM_ADDRESS;
        uintptr_t base   = offset & PAGE_MASK;
        offset &= ~PAGE_MASK;
        assert(offset + sizeof(T) <=  // TODO <= or < ?
               PAGE_SIZE);

        memmap = mmap(               //
            NULL,                    // address
            PAGE_SIZE,               // length
            PROT_READ | PROT_WRITE,  // prot
            MAP_SHARED,              // flags
            mem_fd,                  // file descriptor
            base                     // offset
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
    ~BaremetalShared() {
        munmap(memmap, PAGE_SIZE);
        close(mem_fd);
    }
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
    int mem_fd;
#else
    std::unique_ptr<volatile T> structdata;
#endif
};