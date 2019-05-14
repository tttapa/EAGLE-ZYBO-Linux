#pragma once

#include <cryptoImplementation.hpp>

class HWImplementation : public CryptoImplementation {

  public:
    virtual BitString hash(const BitString &message) const override;

    virtual BitString decrypt(const BitString &key, const BitString &nonce,
                              const BitString &associatedData,
                              const BitString &cipherText,
                              const BitString &tag) const override;
};