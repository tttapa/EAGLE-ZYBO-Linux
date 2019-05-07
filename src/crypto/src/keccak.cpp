#include <keccak.hpp>

#include <cryptoPoller.hpp>

BitString hash(BitString digest) {
    digest.concatenateAndAddMultiRatePadding(0x02, 16, digest.getLength());

    std::vector<BitString> blockDigest;
    digest.splitBlocks(blockDigest);

    CryptoPoller cryptoPoller;
    cryptoPoller.reset();

    for (uint16_t i = 0; i + 1 < (uint16_t) blockDigest.size(); i++)
        cryptoPoller.hashAbsorb(blockDigest[i].toUint32());

    BitString key(cryptoPoller.hashAbsorb(
                      (blockDigest[blockDigest.size() - 1]).toUint32()),
                  16);
    key.reserve(96);

    for (uint8_t i = 0; i < 5; i++)
        key.concatenate(BitString(cryptoPoller.hashSqueeze(), 16));

    return key;
}