#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <MulticastSender.hpp>

TEST(MulticastSender, send) {
    const char *group = "239.0.0.1";
    int port          = 77585;

    // create what looks like an ordinary UDP socket
    int serversock = socket(AF_INET, SOCK_DGRAM, 0);
    if (serversock < 0)
        FAIL() << "Error opening server socket: " << errno;

    // allow multiple sockets to use the same PORT number
    u_int enable = 1;
    if (setsockopt(serversock, SOL_SOCKET, SO_REUSEADDR, &enable,
                   sizeof(enable)) < 0)
        FAIL() << "Error setting SO_REUSEADDR on server socket: " << errno;

    // set up destination address
    struct sockaddr_in addr {};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(port);

    // bind to receive address
    if (bind(serversock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        FAIL() << "Error binding server socket: " << errno;

    // use setsockopt() to request that the kernel join a multicast group
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(serversock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq,
                   sizeof(mreq)) < 0)
        FAIL() << "Error joining multicast group server: " << errno;

    // create sender
    MulticastSender sender = {group, port};
    sender.begin();
    std::vector<uint8_t> data = {0x00, 0x10, 0x20, 0x00, 0xAA, 0x55};
    sender.send(data);

    // receive data
    std::vector<uint8_t> received(8);
    socklen_t addrlen = sizeof(addr);
    ssize_t nbytes = recvfrom(serversock, received.data(), received.size(), 0,
                              (struct sockaddr *) &addr, &addrlen);
    if (nbytes < 0)
        FAIL() << "Error receiving data: " << errno;

    received.resize(nbytes);

    ASSERT_EQ(data, received);
}