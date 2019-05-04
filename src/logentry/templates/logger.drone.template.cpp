#include <LogEntry.h>

void log_data() {
        static volatile LogEntry &logEntry = new ((void *) LOG) LogEntry;

$cpp_assignments
}