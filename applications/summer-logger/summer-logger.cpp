#include <ANSIColors.hpp>
#include <PerfTimer.hpp>
#include <UDPSender.hpp>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  // usleep

#include <logger/ParsedLogEntry.hpp>

const size_t PAGE_SIZE      = getpagesize();
const uintptr_t PAGE_MASK   = ~((uintptr_t) PAGE_SIZE - 1);
const uintptr_t OFFSET_MASK = (uintptr_t) PAGE_SIZE - 1;

using namespace std;
using namespace chrono_literals;

namespace SharedMemory {

/// The start address of the shared memory region in the On-Chip Memory (OCM).
constexpr uintptr_t START_ADDRESS = 0xFFFF0000;
/// The last valid address of the shared memory region in th On-Chip Memory.
constexpr uintptr_t LAST_ADDRESS = 0xFFFFFFFF;

}  // namespace SharedMemory

void loop();

int main() {
    uintptr_t base = SharedMemory::START_ADDRESS & PAGE_MASK;
    // uintptr_t offset = SharedMemory::START_ADDRESS & OFFSET_MASK;
    size_t mmapsize =
        SharedMemory::LAST_ADDRESS - SharedMemory::START_ADDRESS + 1;
    assert(mmapsize % PAGE_SIZE == 0);

    // Open /dev/mem
    int mem_fd = open(   //
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
    // Map the hardware address of the shared memory region into the virtual
    // address space of the program.
    // Offset should be aligned to a page, and size should be a multiple of
    // the page size.
    void *memmap = mmap(         //
        nullptr,                 // address
        mmapsize,                // length
        PROT_READ | PROT_WRITE,  // protection
        MAP_SHARED,              // flags
        mem_fd,                  // file descriptor
        base                     // offset
    );
    if (memmap == MAP_FAILED) {
        std::ostringstream oss;
        oss << "mmap(" << std::hex << std::showbase << base << std::dec
            << std::noshowbase << ") failed (" << errno << ")";
        std::cerr << ANSIColors::redb << "mmap(" << std::hex << std::showbase
                  << base << std::dec << std::noshowbase << ") failed ("
                  << errno << ")" << ANSIColors::reset << std::endl;
        throw std::runtime_error(oss.str());
    }
    close(mem_fd);
    uintptr_t memmapp = reinterpret_cast<uintptr_t>(memmap);

    /// The size of the logging data. Linux sets it to 0 when it's done reading,
    /// and Baremetal sets it to the correct size when it's finished writing.
    volatile uint32_t &size = *reinterpret_cast<uint32_t *>(memmapp);
    /// A pointer to the buffer where the logging data is written.
    volatile uint8_t *const buffer = reinterpret_cast<uint8_t *>(memmapp + 4);

    UDPSender sender = {"239.0.0.5", 5000};
    sender.begin();
    while (1) {
        if (size != 0) {
            std::vector<uint8_t> vectorBuffer(buffer, buffer + size);
            size               = 0;
            ParsedLogEntry log = {vectorBuffer.data(), vectorBuffer.size()};
            // if (log.contains("altitude-measurement"))
            //     cout << log["altitude-measurement"].get<float>() << endl;
            if (log.contains("altitude.reference")) {
                cout << "altitude.reference: "
                     << log["altitude.reference"].get<float>() << '\t'
                     << "altitude.control: "
                     << log["altitude.control"].get<float>() << '\t'
                     << "altitude.state.z: "
                     << log["altitude.state.z"].get<float>() << '\t'
                     << "altitude.measurement: "
                     << log["altitude.measurement"].get<float>() << '\t'
                     << endl;
            }
            sender.send(vectorBuffer);
        }
    }
}