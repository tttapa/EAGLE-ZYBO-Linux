#include <ketje.hpp>

void Ketje::decrypt(const BitString &associatedData,
                    const BitString &cipherText, const BitString &tag,
                    const BitString &nonce) {}

void Ketje::initialize(const BitString &nonce) {
    BitString bitstring(key.getLength() + 16);
    bitstring.concatenate(key);
    bitstring.addMultiRatePadding(200, bitstring.getLength());
    std::array<uint32_t, 10> keypack;
    bitstring.split(keypack);
    for (int i = 0; i < 10; i++) {
        cryptoPoller.write(9 - i, keypack[i]);
    }
    cryptoPoller.start();
}

BitString Ketje::unwrap(const BitString &associatedData,
                        const BitString &cipherText, const BitString &tag) {
    std::vector<BitString> blockAssociatedData;
    associatedData.splitBlock(blockAssociatedData);
    for (int i = 0; i < blockAssociatedData.size() - 1; i++) {
        cryptoPoller.normalStep(
            blockAssociatedData[i]
                .concatenateAndAddMultiRatePadding(
                    0x00, 20, blockAssociatedData[i].getLength())
                .toUint32());
    }
    uint32_t result = cryptoPoller.normalStep(
        blockAssociatedData[blockAssociatedData.size() - 1]
            .concatenateAndAddMultiRatePadding(
                0x10, 20,
                blockAssociatedData[blockAssociatedData.size() - 1].getLength())
            .toUint32());
    std::vector<BitString> blockCipherText;
    cipherText.splitBlock(blockCipherText);
    
    for (int i = 0; i < blockCipherText.size() - 1; i++) {
        result = cryptoPoller.xorStep(
            blockCipherText[i]
                .concatenateAndAddMultiRatePadding(
                    0x11, 20, blockCipherText[i].getLength())
                .toUint32());
    }
}