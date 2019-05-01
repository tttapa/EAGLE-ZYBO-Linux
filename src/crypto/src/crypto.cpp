#include <crypto.hpp>

#include <stdexcept>

// TODO: unimplemented stub
CryptoInstruction decrypt(const std::vector<uint8_t> &) {
    throw std::runtime_error(std::string("ERROR: ") + __PRETTY_FUNCTION__ +
                             " not implemented: " __FILE__ ":" +
                             std::to_string(__LINE__));
}