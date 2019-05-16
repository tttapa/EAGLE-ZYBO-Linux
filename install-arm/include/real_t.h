#ifndef REAL_T_H
#define REAL_T_H

#ifdef __cplusplus
#define ADD_CPP_REF(x) (&x)
#else
#define ADD_CPP_REF(x) x
#endif

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

#endif /* REAL_T_H */