#include <SharedMem.hpp>

#ifdef ZYBO
size_t SharedMemReferenceCounter::count = 0;
int SharedMemReferenceCounter::mem_fd   = 0;
#endif