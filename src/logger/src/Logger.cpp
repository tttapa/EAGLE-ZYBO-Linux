#include <Logger.hpp>

void Logger::begin() { sender.begin(); }

void Logger::update() {
    if (acLogEntry->isDoneWriting()) {
        LogEntry logEntry = acLogEntry->read();
        sender.send(logEntry);
    }
}