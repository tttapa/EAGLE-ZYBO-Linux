#pragma once

#include <BaremetalCommunicationDef.hpp>
#include <LogEntry.h>
#include <UDPSender.hpp>
#include <SharedMem.hpp>

/**
 * @brief   A class that reads the logging data from shared memory, and sends it
 *          it over UDP.
 */
class Logger {
  public:
    /**
     * @brief   Construct a new Logger object.
     * 
     * @param   group
     *          The group address to send to.
     * @param   port 
     *          The port number to send to.
     */
    Logger(const char *group, int port) : sender{group, port} {}
    Logger(Logger &&) = default;

    /**
     * @brief   Initialize the logger.
     */
    void begin();
    /**
     * @brief   Update the logger: check if there is new logging data from 
     *          Baremetal, and if so, send it over UDP.
     */
    void update();
    /**
     * @brief   Check if the logger is initialized by Baremetal.
     * 
     * @see     SharedStruct::isInitialized
     */
    bool isInitialized() const { return acLogEntry->isInitialized(); }
    /**
     * @brief   Make sure that the logger is initialized by Baremetal.
     * 
     * @see     SharedStruct::checkInitialized
     */
    void checkInitialized() const { acLogEntry->checkInitialized(); }

  private:
    UDPSender sender;
    SharedMemory<AccessControlledLogEntry> acLogEntry;
};