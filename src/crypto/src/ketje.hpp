#pragma once

#include <bitString.hpp>
#include <cryptoPoller.hpp>

class Ketje {
  public:
    Ketje(const BitString &key) : key(key) {}

    void decrypt(const BitString &associatedData, const BitString &cipherText,
                 const BitString &tag, const BitString &nonce);

  private:
    BitString key;
    CryptoPoller cryptoPoller;

    void initialize(const BitString &nonce);

    BitString unwrap(const BitString &associatedData,
                     const BitString &cipherText, const BitString &tag);
};