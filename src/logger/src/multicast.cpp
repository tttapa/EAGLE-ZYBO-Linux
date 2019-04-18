#include <arpa/inet.h>
#include <cerrno>
#include <cstring>  // memset
#include <iostream>
#include <multicast.hpp>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>  // close

MulticastSender::MulticastSender(const char *group, int port) : address{} {
    memset(&address, 0, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = inet_addr(group);
    address.sin_port        = htons(port);
}

void MulticastSender::begin() {
    socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (socket < 0) {
        switch (errno) {
            case EACCES:
                throw std::runtime_error(
                    "EACCES: Permission to create a socket of the "
                    "specified type and/or "
                    "protocol is denied.");
            case EAFNOSUPPORT:
                throw std::runtime_error(
                    "EAFNOSUPPORT: The implementation does not support the "
                    "specified address family.");
            case EINVAL:
                throw std::runtime_error(
                    "EINVAL: Unknown protocol, or protocol family not "
                    "available. Invalid flags in type.");
            case EMFILE:
                throw std::runtime_error(
                    "EMFILE: The per-process limit on the number of open "
                    "file descriptors has been reached.");
            case ENFILE:
                throw std::runtime_error(
                    "ENFILE: The system-wide limit on the total number of "
                    "open files has been reached.");
            case ENOMEM:
            case ENOBUFS:
                throw std::runtime_error(
                    "ENOMEM or ENOBUFS: Insufficient memory is available.  "
                    "The socket cannot be created until sufficient "
                    "resources are freed.");
            case EPROTONOSUPPORT:
                throw std::runtime_error(
                    "EPROTONOSUPPORT: The protocol type or the specified "
                    "protocol is not supported within this domain.");
            default:
                throw std::runtime_error("Unknown error opening socket: " +
                                         std::to_string(errno));
        }
    }
}

MulticastSender::~MulticastSender() {
#if 0
    int result = shutdown(socket, 2);
    if (result < 0) {
        switch (errno) {
            case EBADF:
                std::cerr << "EBADF: socket is not a valid file descriptor."
                          << std::endl;
                break;
            case ENOTSOCK:
                std::cerr << "ENOTSOCK: socket is not a socket." << std::endl;
                break;
            case ENOTCONN:
                std::cerr << "ENOTCONN: socket is not connected." << std::endl;
                break;
            default:
                std::cerr << "Unknown error shutting down socket: " << errno
                          << std::endl;
        }
    }
#endif

    close(socket);
}

size_t MulticastSender::send(const std::vector<uint8_t> &data) {
    ssize_t nbytes = sendto(socket,                    //
                            data.data(), data.size(),  //
                            0,                         //
                            (struct sockaddr *) &address, sizeof(address));
    if (nbytes < 0)
        throw std::runtime_error("Error sending data: " +
                                 std::to_string(errno));
    return nbytes;
}