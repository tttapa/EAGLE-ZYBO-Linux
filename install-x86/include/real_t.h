#ifndef REAL_T_H
#define REAL_T_H

#if 0
#include <stdint.h>
#if UINTPTR_MAX == 0xffffffff
/* 32-bit */
typedef float real_t;
#elif UINTPTR_MAX == 0xffffffffffffffff
/* 64-bit */
typedef double real_t;
#else
#error Architecture not supported
#endif
#else
typedef float real_t;
#endif

#endif /* REAL_T_H */