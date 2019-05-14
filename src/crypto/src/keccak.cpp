#include <keccak.hpp>

#include <cryptoPoller.hpp>

BitString keccak(BitString message) {
    message.concatenateAndAddMultiRatePadding(0x02, 16, message.getLength());

    std::vector<BitString> blockMessage;
    message.splitBlocks(blockMessage);

    CryptoPoller cryptoPoller;
    cryptoPoller.reset();

    for (uint16_t i = 0; i + 1 < (uint16_t) blockMessage.size(); i++)
        cryptoPoller.hashAbsorb(blockMessage[i].toUint32());

    BitString key(cryptoPoller.hashAbsorb(
                      (blockMessage[blockMessage.size() - 1]).toUint32()),
                  16);
    key.reserve(96);

    for (uint8_t i = 0; i < 5; i++)
        key.concatenate(BitString(cryptoPoller.hashSqueeze(), 16));

    return key;
}