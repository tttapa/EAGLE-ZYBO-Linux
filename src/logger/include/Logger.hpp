#pragma once

#include <BaremetalCommunicationDef.hpp>
#include <LogEntry.h>
#include <SharedMem.hpp>
#include <UDPSender.hpp>
#include <vector>

/**
 * @brief   A class that reads the logging data from shared memory, and sends it
 *          it over UDP.
 */
class Logger {
  public:
    /**
     * @brief   Construct a new Logger object that sends out logging packets to
     *          a single address (multicast or unicast).
     * 
     * @param   addr
     *          The (group) address to send to.
     * @param   port 
     *          The port number to send to.
     */
    Logger(const char *addr, int port) : senders{{addr, port}} {}
    /**
     * @brief   Construct a new Logger object that sends out logging packets to
     *          multiple addresses (multicast or unicast).
     * 
     * @param   senders
     *          A vector of senders.
     */
    Logger(const std::vector<UDPSender> &senders) : senders{senders} {}
    /**
     * @brief   Move constructor.
     */
    Logger(Logger &&other) {
        std::swap(this->senders, other.senders);
        std::swap(this->acLogEntry, other.acLogEntry);
    };

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
    std::vector<UDPSender> senders;
    SharedMemory<AccessControlledLogEntry> acLogEntry = {};
};