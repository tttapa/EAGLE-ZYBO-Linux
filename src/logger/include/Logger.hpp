#pragma once

#include <BaremetalCommunicationDef.hpp>
#include <LogEntry.h>
#include <MulticastSender.hpp>
#include <SharedMem.hpp>

class Logger {
  public:
    Logger(const char *group, int port) : sender{group, port} {}

    void begin();
    void update();
    bool isInitialized() const { return acLogEntry->isInitialized(); }
    void checkInitialized() const { acLogEntry->checkInitialized(); }

  private:
    MulticastSender sender;
    SharedMemory<AccessControlledLogEntry> acLogEntry;
};