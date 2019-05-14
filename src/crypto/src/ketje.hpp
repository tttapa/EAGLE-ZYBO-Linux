#pragma once

#include <bitString.hpp>
#include <cryptoPoller.hpp>

class Ketje {

  private:
    BitString key;
    CryptoPoller cryptoPoller;

  public:
    Ketje(const BitString &key) : key(key) {}

    void initialize(const BitString &nonce);

    BitString unwrap(const BitString &associatedData,
                     const BitString &cipherText, const BitString &tag);
};