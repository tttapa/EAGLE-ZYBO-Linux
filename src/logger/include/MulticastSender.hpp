#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <vector>

class MulticastSender {
  public:
    MulticastSender(const char *group, int port);
    ~MulticastSender();

    void begin();
    size_t send(const uint8_t *data, size_t size);
    size_t send(const std::vector<uint8_t> &data) {
        return send(data.data(), data.size());
    }
    template <class T>
    size_t send(const T &data) {
        return send(reinterpret_cast<const uint8_t *>(&data), sizeof(data));
    }

  private:
    struct sockaddr_in address;
    int socket;
};