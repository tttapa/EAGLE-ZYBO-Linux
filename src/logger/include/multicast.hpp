#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <vector>

class MulticastSender {
  public:
    MulticastSender(const char *group, int port);
    ~MulticastSender();

    void begin();
    size_t send(const std::vector<uint8_t> &data);

  private:
    struct sockaddr_in address;
    int socket;
};