#include <ThreadedLogger.hpp>

ThreadedLogger::ThreadedLogger(const char *address, int port)
    : logger(address, port) {}

ThreadedLogger::~ThreadedLogger() {
    if (started) {
        loggerStop.set_value();  // Tell the logger thread to stop
        thread.join();           // TODO: is this necessary?
    }
}

void ThreadedLogger::begin(std::chrono::microseconds interval) {
    if (started)
        throw std::runtime_error("Error: Logger is already running!");

    using namespace std::chrono_literals;
    auto runLogger = [this, interval](auto loggerStop) {
        while (loggerStop.wait_for(interval) != std::future_status::ready)
            logger.update();  // TODO: should I catch exceptions here?
    };

    logger.begin();
    thread  = std::thread(runLogger, move(loggerStop.get_future()));
    started = true;
}