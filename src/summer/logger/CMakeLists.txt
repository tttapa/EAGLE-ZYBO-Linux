add_library(summer-logger 
    src/LogEntryIterator.cpp
    src/Loggable.cpp
    src/Logger.cpp
    src/LoggerHelpers.hpp
    src/ParsedLogEntry.cpp
)
target_include_directories(summer-logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(summer-logger summer-math)