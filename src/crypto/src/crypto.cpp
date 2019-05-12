#include <bitString.hpp>
#include <crypto.hpp>

CryptoInstruction decrypt(const std::vector<uint8_t> &qrCode) {
    return CryptoInstruction(CryptoInstruction::GOTO, 0, 0);
}