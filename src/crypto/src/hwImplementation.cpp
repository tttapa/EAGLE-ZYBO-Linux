#include <hwImplementation.hpp>

#include <keccak.hpp>
#include <ketje.hpp>

BitString HWImplementation::hash(const BitString &message) {
    return keccak(message);
}

BitString HWImplementation::decrypt(const BitString &key,
                                    const BitString &nonce,
                                    const BitString &associatedData,
                                    const BitString &cipherText,
                                    const BitString &tag) {
    Ketje ketje(key);

    ketje.initialize(nonce);
    return ketje.unwrap(associatedData, cipherText, tag);
}