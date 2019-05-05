#pragma once

#include <Logger.hpp>
#include <chrono>
#include <future>

/**
 * @brief   A class that manages a Logger and continuously updates it in a
 *          different thread, until it is destroyed.
 */
class ThreadedLogger {
  public:
    /**
     * @brief   Construct a new ThreadedLogger object.
     * 
     * @param   logger
     *          The logger to manage.
     */
    ThreadedLogger(Logger &&logger);

    /**
     * @brief   Destroy the ThreadedLogger object. This gracefully stops the 
     *          child thread.
     */
    ~ThreadedLogger();

    /**
     * @brief   Spawn a new thread for the logger. The logger will update at the
     *          given interval.
     * 
     * @param   interval
     *          The interval of the update rate.
     */
    void begin(std::chrono::microseconds interval);

    /// @see    Logger::isInitialized
    bool isInitialized() const { return logger.isInitialized(); }
    /// @see    Logger::checkInitialized
    void checkInitialized() const { logger.checkInitialized(); }

  private:
    bool started = false;
    Logger logger;
    std::promise<void> loggerStop;
    std::thread thread;
};