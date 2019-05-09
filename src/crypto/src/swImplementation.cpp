#include <swImplementation.hpp>

#include <EagleCrypt.h>
#include <cryptoException.hpp>

BitString SWImplementation::hash(const BitString &message) {
    unsigned char *data = nullptr;

    try {
        uint16_t nbBytes = message.getLength() / 8;
        data             = new unsigned char[nbBytes];
        message.toByteArray(data, nbBytes);

        unsigned char digest[96 / 8];

        if (::hash(data, nbBytes, digest, 96 / 8) != 0)
            throw CryptoException(
                CryptoException::ExceptionType::UNSUCCESSFUL_DECODE_EXCEPTION);

        delete[] data;
        data = nullptr;

        std::vector<uint8_t> vectorDigest;
        vectorDigest.reserve(96 / 8);
        for (uint8_t i = 0; i < 96 / 8; i++)
            vectorDigest.push_back((uint8_t) digest[i]);

        return vectorDigest;
    } catch (...) {
        if (data)
            delete[] data;

        throw;
    }
}

BitString SWImplementation::decrypt(const BitString &key,
                                    const BitString &nonce,
                                    const BitString &associatedData,
                                    const BitString &cipherText,
                                    const BitString &tag) {
    unsigned char *charKey     = nullptr;
    unsigned char *charNonce   = nullptr;
    unsigned char *ad          = nullptr;
    unsigned char *ct          = nullptr;
    unsigned char *pt          = nullptr;
    unsigned char *producedTag = nullptr;

    try {
        charKey = new unsigned char[key.getLength() / 8];
        key.toByteArray(charKey, key.getLength() / 8);

        charNonce = new unsigned char[nonce.getLength() / 8];
        nonce.toByteArray(charNonce, nonce.getLength() / 8);

        if (::init(charKey, key.getLength(), charNonce, nonce.getLength()) != 0)
            throw CryptoException(
                CryptoException::ExceptionType::UNSUCCESSFUL_DECODE_EXCEPTION);

        delete[] charKey;
        charKey = nullptr;
        delete[] charNonce;
        charNonce = nullptr;

        ad = new unsigned char[associatedData.getLength() / 8];
        associatedData.toByteArray(ad, associatedData.getLength() / 8);

        ct = new unsigned char[cipherText.getLength() / 8];
        cipherText.toByteArray(ct, cipherText.getLength() / 8);

        pt          = new unsigned char[cipherText.getLength() / 8];
        producedTag = new unsigned char[tag.getLength() / 8];

        if (::decrypt(ad, associatedData.getLength() / 8, ct,
                      cipherText.getLength() / 8, pt, producedTag,
                      tag.getLength() / 8) != 0)
            throw CryptoException(
                CryptoException::ExceptionType::UNSUCCESSFUL_DECODE_EXCEPTION);

        delete[] ad;
        ad = nullptr;
        delete[] ct;
        ct = nullptr;

        std::vector<uint8_t> vectorProducedTag;
        vectorProducedTag.reserve(tag.getLength() / 8);
        for (uint8_t i = 0; i < tag.getLength() / 8; i++)
            vectorProducedTag.push_back((uint8_t) producedTag[i]);

        if (BitString(vectorProducedTag) != tag)
            throw CryptoException(
                CryptoException::ExceptionType::UNSUCCESSFUL_DECODE_EXCEPTION);

        delete[] producedTag;
        producedTag = nullptr;

        std::vector<uint8_t> vectorPlainText;
        vectorPlainText.reserve(cipherText.getLength() / 8);
        for (uint8_t i = 0; i < cipherText.getLength() / 8; i++)
            vectorPlainText.push_back((uint8_t) pt[i]);

        delete[] pt;
        pt = nullptr;

        return vectorPlainText;
    } catch (...) {
        if (charKey)
            delete[] charKey;
        if (charNonce)
            delete[] charNonce;
        if (ad)
            delete[] ad;
        if (ct)
            delete[] ct;
        if (pt)
            delete[] pt;
        if (producedTag)
            delete[] producedTag;

        throw;
    }
}