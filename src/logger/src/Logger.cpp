#include <Logger.hpp>

void Logger::begin() {
    for (auto &sender : senders)
        sender.begin();
}

void Logger::update() {
    if (acLogEntry->isDoneWriting()) {
        LogEntry logEntry = acLogEntry->read();
        for (auto &sender : senders)
            sender.send(logEntry);
        std::cout << "Logger: framecounter = " << logEntry.framecounter
                  << std::endl;
    }
}