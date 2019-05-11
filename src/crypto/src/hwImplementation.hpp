#pragma once

#include <bitString.hpp>

namespace HWImplementation {

BitString hash(const BitString &message);

BitString decrypt(const BitString &key, const BitString &nonce,
                  const BitString &associatedData, const BitString &cipherText,
                  const BitString &tag);
}  // namespace HWImplementation