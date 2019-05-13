#include <crypto.hpp>

#include <bitString.hpp>
#include <cryptoConstants.hpp>
#include <fstream>
#include <hwImplementation.hpp>
#include <string>
#include <swImplementation.hpp>

CryptoInstruction tryDecrypt(const std::vector<uint8_t> &qrCode,
                             Implementation implementation);
BitString hash(const BitString &message, Implementation implementation);
BitString decrypt(const BitString &key, const BitString &nonce,
                  const BitString &associatedData, const BitString &cipherText,
                  const BitString &tag, Implementation implementation);

CryptoInstruction decrypt(const std::vector<uint8_t> &qrCode,
                          Implementation implementation) {
    for (uint8_t i = 0; i < nbTrials; i++) {
        try {
            return tryDecrypt(qrCode, implementation);
        }
        // Try again if crypto fails with a timeout exception.
        // Don't try again if any other CryptoException occurs.
        catch (CryptoException &exception) {
            if (exception.getExceptionType() ==
                CryptoException::ExceptionType::TIMEOUT_EXCEPTION)
                continue;

            throw;
        }
        // Don't try again if any other exception occurs.
        catch (std::out_of_range &exception) {
            throw CryptoException(
                CryptoException::ExceptionType::UNSUCCESSFUL_DECODE_EXCEPTION);
        } catch (std::exception &exception) {
            throw CryptoException(
                CryptoException::ExceptionType::UNKNOWN_ERROR_EXCEPTION,
                exception.what());
        } catch (...) {
            throw CryptoException(
                CryptoException::ExceptionType::UNKNOWN_ERROR_EXCEPTION);
        }
    }

    // Throw a timeout exception if too many attemps to decrypt
    // the given QR code fail.
    throw CryptoException(CryptoException::ExceptionType::TIMEOUT_EXCEPTION);
}

CryptoInstruction tryDecrypt(const std::vector<uint8_t> &qrCode,
                             Implementation implementation) {
    // Read drone master key from the file.
    char key[dmKeySizeInBytes];
    std::ifstream keyReader(dmKeyPath, std::ios::binary);
    keyReader.read(key, dmKeySizeInBytes);
    keyReader.close();

    // Convert drone master key to vector.
    std::vector<uint8_t> vectorDmKey;
    vectorDmKey.reserve(dmKeySizeInBytes);
    for (uint16_t i = 0; i < dmKeySizeInBytes; i++)
        vectorDmKey.push_back(key[i]);

    BitString dmKey(vectorDmKey);

    BitString nonce(qrCode, 0, 10);
    BitString ad(qrCode, 10, 15);

    // Create and hash key material from the QR code.
    BitString salt(qrCode, 10, 14);
    BitString keyMaterial(dmKey);
    keyMaterial.reserve(160);
    keyMaterial.concatenate(salt);

    BitString wpKey(hash(keyMaterial, implementation));

    uint8_t nbInstructionsLeft = qrCode.at(14);
    int16_t currentIndex       = 15;
    while (nbInstructionsLeft > 0) {
        nbInstructionsLeft--;

        int8_t nbBytes = qrCode.at(currentIndex);
        currentIndex += 1;
        BitString cipherText(qrCode, currentIndex, currentIndex + nbBytes - 4);

        currentIndex += (nbBytes - 4);
        BitString tag(qrCode, currentIndex, currentIndex + 4);
        currentIndex += 4;

        BitString plainText;
        try {
            plainText =
                decrypt(wpKey, nonce, ad, cipherText, tag, implementation);
        } catch (CryptoException &exception) {
            if (exception.getExceptionType() ==
                CryptoException::ExceptionType::TIMEOUT_EXCEPTION)
                throw;
            else
                continue;
        }

        return plainText.toCryptoInstruction();
    }

    throw CryptoException(
        CryptoException::ExceptionType::UNSUCCESSFUL_DECODE_EXCEPTION);
}

BitString hash(const BitString &message, Implementation implementation) {
    switch (implementation) {
        case Implementation::SWImplementation:
            return SWImplementation::hash(message);
            break;
        case Implementation::HWImplementation:
            return HWImplementation::hash(message);
            break;
        default:
            throw CryptoException(
                CryptoException::ExceptionType::UNKNOWN_ERROR_EXCEPTION,
                "The given implementation is not valid. Choose either software "
                "or hardware implementation.");
            break;
    }
}

BitString decrypt(const BitString &key, const BitString &nonce,
                  const BitString &associatedData, const BitString &cipherText,
                  const BitString &tag, Implementation implementation) {
    switch (implementation) {
        case Implementation::SWImplementation:
            return SWImplementation::decrypt(key, nonce, associatedData,
                                             cipherText, tag);
            break;
        case Implementation::HWImplementation:
            return HWImplementation::decrypt(key, nonce, associatedData,
                                             cipherText, tag);
            break;
        default:
            throw CryptoException(
                CryptoException::ExceptionType::UNKNOWN_ERROR_EXCEPTION,
                "The given implementation is not valid. Choose either software "
                "or hardware implementation.");
            break;
    }
}