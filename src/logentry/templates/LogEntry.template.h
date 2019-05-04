#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
#include <algorithm> // copy
#include <cstdint>   // uint32_t, int32_t
#include <iterator>  // begin, end

$typedefs
#else
#include <stdint.h> // uint32_t, int32_t
#endif

#ifdef __cplusplus
struct LogEntry {
#else
typedef struct {
#endif 
$members
#ifdef __cplusplus

$cpp_methods
#endif

#ifdef __cplusplus
};
#else
} LogEntry;
#endif

#if !defined(__cplusplus) || defined(LOGGER_INCLUDE_C_WRAPPERS)
#ifdef __cplusplus
extern "C" {
#endif

$c_prototypes
#ifdef __cplusplus
}
#endif
#endif

#endif  // LOGGER_H 
