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

  private:
    MulticastSender sender;
    SharedMemory<AccessControlledLogEntry> acLogEntry;
};