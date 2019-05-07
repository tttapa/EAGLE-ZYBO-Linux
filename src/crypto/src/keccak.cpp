#include <keccak.hpp>
#include <cryptoPoller.hpp>

BitString hash(BitString digest) {
    digest.concatenateAndAddMultiRatePadding(0x02, 16, digest.getLength());
    std::vector<BitString> blockDigest;
    digest.splitBlocks(blockDigest);
    
    for (BitString digestBlock : blockDigest) {

    }
}