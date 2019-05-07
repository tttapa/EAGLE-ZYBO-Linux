#include <keccak.hpp>
#include <cryptoPoller.hpp>

BitString hash(BitString digest) {
    digest.concatenateAndAddMultiRatePadding(0x02, 16, digest.getLength());
    //digest.addMultiRatePadding(16, digest.getLength());
    std::vector<BitString> blockDigest;
    digest.splitBlocks(blockDigest);
    CryptoPoller cryptopoller;
    cryptopoller.reset();

    std::cout << std::hex << "Data to absorb: " << digest;
    std::cout << std::dec << "\nData to absorb: " << digest;

    BitString key;

    for (BitString &digestBlock : blockDigest)
        key = BitString(cryptopoller.hashAbsorb(digestBlock.toUint32()), 16);
    
    //BitString key(cryptopoller.read(9), 16);

    for (uint8_t i = 0; i < 5; i++)
        key.concatenate(BitString(cryptopoller.hashSqueeze(), 16));
    
    return key;
}