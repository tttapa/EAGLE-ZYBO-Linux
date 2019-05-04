#pragma once

#include <LogEntry.h>
#include <MulticastSender.hpp>
#include <SharedMem.hpp>
#include <BaremetalCommunicationDef.hpp>

class Logger {
  public:
    Logger(const char *group, int port) : sender{group, port} {}

    void update();

  private:
    MulticastSender sender;
    BaremetalShared<AccessControlledLogEntry> acLogEntry;
};