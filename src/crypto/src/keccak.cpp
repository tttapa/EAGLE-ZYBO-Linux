#include <keccak.hpp>
#include <cryptoPoller.hpp>

BitString hash(BitString digest) {
    digest.concatenateAndAddMultiRatePadding(0x02, 16, digest.getLength());
    std::vector<BitString> blockDigest;
    digest.splitBlocks(blockDigest);
    CryptoPoller cryptopoller;
    cryptopoller.reset();
    for (BitString &digestBlock : blockDigest)
        cryptopoller.hashAbsorb(digestBlock.toUint32());
    
    BitString key(cryptopoller.read(9), 16);

    for (uint8_t i = 0; i < 5; i++)
        key.concatenate(BitString(cryptopoller.hashSqueeze(), 16));
    
    return key;
}