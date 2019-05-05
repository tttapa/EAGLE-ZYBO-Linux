#pragma once

#include <Logger.hpp>
#include <future>
#include <chrono>

/**
 * @brief   A class that manages a logger and updates it in a different thread.
 */
class ThreadedLogger {
  public:
    /**
     * @brief   Construct a new ThreadedLogger object.
     * 
     * @param   address 
     *          The address to send the logging data to.
     * @param   port 
     *          The port to send the logging data to.
     */
    ThreadedLogger(const char *address, int port);

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

  private:
    bool started = false;
    Logger logger;
    std::promise<void> loggerStop;
    std::thread thread;
};