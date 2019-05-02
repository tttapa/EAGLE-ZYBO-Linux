#include <ketje.hpp>

void Ketje::initialize(const BitString &nonce) {
    BitString bitString(key.getLength() + 16);
    bitString.reserve(200);
    bitString.concatenate(key);
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
    for (int16_t i = 0; i < blockAssociatedData.size() - 1; i++) {
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
    BitString *bitStringResult =
        new BitString(result, blockCipherText[0].getLength());

    for (int i = 0; i < blockCipherText.size() - 1; i++) {
        bitStringResult->xorWith(blockCipherText[i]);
        delete bitStringResult;
        bitStringResult = new BitString(result, blockCipherText[i].getLength());
        result          = cryptoPoller.normalStep(
            bitStringResult
                ->concatenateAndAddMultiRatePadding(
                    0x03, 20, blockCipherText[i].getLength())
                .toUint32());
    }

    BitString bitStringResult(result, blockCipherText[0].getLength());
    bitStringResult.xorWith(blockCipherText[0]);
    result = cryptoPoller.normalStep(
        bitStringResult
            .concatenateAndAddMultiRatePadding(0x03, 20,
                                               blockCipherText[i].getLength())
            .toUint32());
}