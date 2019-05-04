#define LOGGER_INCLUDE_C_WRAPPERS
#include <LogEntry.h>
#include <cstring>  // memcpy

static_assert(sizeof(LogEntry) == sizeof(float) * $size, 
              "Error: packing of LogEntry is incorrect");

$c_functions 
