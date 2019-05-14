#pragma once

#include <bitString.hpp>

class CryptoImplementation {

  public:
    virtual BitString hash(const BitString &message) const = 0;

    virtual BitString decrypt(const BitString &key, const BitString &nonce,
                              const BitString &associatedData,
                              const BitString &cipherText,
                              const BitString &tag) const = 0;
};