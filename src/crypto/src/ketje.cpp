#include <ketje.hpp>

void Ketje::initialize(const BitString &nonce) {
    BitString bitString(key.getLength() + 16);
    bitString.reserve(200);
    bitString.concatenate(key);
    bitString.addSimplePadding(key.getLength() + 16 - 8, key.getLength());
    bitString.concatenate(nonce);
    bitString.addMultiRatePadding(200, bitString.getLength());

    std::array<uint32_t, 10> keypack;
    bitString.split(keypack);

    for (int8_t i = 0; i < 10; i++)
        cryptoPoller.write(9 - i, keypack[i]);

    cryptoPoller.start();
}

BitString Ketje::unwrap(const BitString &associatedData,
                        const BitString &cipherText, const BitString &tag) {
    std::vector<BitString> blockAssociatedData;
    associatedData.splitBlocks(blockAssociatedData);

    for (uint16_t i = 0; i + 1 < (uint16_t) blockAssociatedData.size(); i++) {
        cryptoPoller.normalStep(
            blockAssociatedData[i]
                .concatenateAndAddMultiRatePadding(
                    0x00, 20, blockAssociatedData[i].getLength())
                .toUint32());
    }

    uint32_t result = cryptoPoller.normalStep(
        blockAssociatedData[blockAssociatedData.size() - 1]
            .concatenateAndAddMultiRatePadding(
                0x02, 20,
                blockAssociatedData[blockAssociatedData.size() - 1].getLength())
            .toUint32());

    std::vector<BitString> blockCipherText;
    cipherText.splitBlocks(blockCipherText);

    BitString bitStringResult(result, blockCipherText[0].getLength());

    BitString plainText;
    for (uint16_t i = 0; i + 1 < (uint16_t) blockCipherText.size(); i++) {
        bitStringResult.xorWith(blockCipherText[i]);
        plainText.concatenate(bitStringResult);

        result = cryptoPoller.normalStep(
            bitStringResult
                .concatenateAndAddMultiRatePadding(0x03, 20,
                                                   bitStringResult.getLength())
                .toUint32());
        bitStringResult = BitString(result, blockCipherText[i + 1].getLength());
    }

    bitStringResult.xorWith(blockCipherText[blockCipherText.size() - 1]);
    plainText.concatenate(bitStringResult);

    result =
        cryptoPoller.normalStride(bitStringResult
                                      .concatenateAndAddMultiRatePadding(
                                          0x01, 20, bitStringResult.getLength())
                                      .toUint32());

    std::vector<BitString> blockTag;
    tag.splitBlocks(blockTag);

    for (uint16_t i = 0; i < blockTag.size(); i++) {
        bitStringResult = BitString(result, blockTag[i].getLength());

        if (bitStringResult != blockTag[i])
            throw CryptoException(
                CryptoException::ExceptionType::UNSUCCESSFUL_DECODE_EXCEPTION);

        if (i + 1 == (uint16_t) blockTag.size())
            break;

        result = cryptoPoller.normalStep(0x00'00'20'08);
    }

    return plainText;
}