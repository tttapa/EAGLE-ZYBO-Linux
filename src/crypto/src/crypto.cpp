#include <crypto.hpp>

#include <bitString.hpp>
#include <cryptoConstants.hpp>
#include <fstream>
#include <hwImplementation.hpp>
#include <string>
#include <swImplementation.hpp>

CryptoInstruction tryDecrypt(const std::vector<uint8_t> &qrCode,
                             const CryptoImplementation &implementation);

CryptoInstruction decrypt(const std::vector<uint8_t> &qrCode,
                          const CryptoImplementation &implementation) {
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
                             const CryptoImplementation &implementation) {
    // Read drone master key from the file.
    char key[dmKeySizeInBytes];
    std::ifstream keyReader(dmKeyPath, std::ios::binary);
    if (keyReader.fail())
        throw CryptoException(
            CryptoException::ExceptionType::UNKNOWN_ERROR_EXCEPTION,
            "No drone master key available.");

    keyReader.read(key, dmKeySizeInBytes);
    keyReader.close();

    // Convert drone master key to vector.
    std::vector<uint8_t> vectorDmKey;
    vectorDmKey.reserve(dmKeySizeInBytes);
    for (uint16_t i = 0; i < dmKeySizeInBytes; i++)
        vectorDmKey.push_back(key[i]);

    return tryDecrypt(qrCode, implementation, vectorDmKey);
}

CryptoInstruction tryDecrypt(const std::vector<uint8_t> &qrCode,
                             const CryptoImplementation &implementation, 
                             const std::vector<uint8_t> &vectorDmKey) {
    BitString dmKey(vectorDmKey);

    BitString nonce(qrCode, 0, 10);
    BitString ad(qrCode, 10, 15);

    // Create and hash key material from the QR code.
    BitString salt(qrCode, 10, 14);
    BitString keyMaterial(dmKey);
    keyMaterial.reserve(160);
    keyMaterial.concatenate(salt);

    BitString wpKey(implementation.hash(keyMaterial));

    uint8_t nbInstructionsLeft = qrCode.at(14);
    uint16_t currentIndex      = 15;
    while (nbInstructionsLeft > 0) {
        nbInstructionsLeft--;

        uint8_t nbBytes = qrCode.at(currentIndex);
        currentIndex += 1;
        BitString cipherText(qrCode, currentIndex, currentIndex + nbBytes - 4);

        currentIndex += (nbBytes - 4);
        BitString tag(qrCode, currentIndex, currentIndex + 4);
        currentIndex += 4;

        try {
            return implementation.decrypt(wpKey, nonce, ad, cipherText, tag)
                .toCryptoInstruction();
        }
        // Only try the next instruction if there is no problem
        // with the FPGA.
        catch (CryptoException &exception) {
            if (exception.getExceptionType() ==
                CryptoException::ExceptionType::TIMEOUT_EXCEPTION)
                throw;
        }
    }

    // Throw an exception if no instruction in the QR code is valid.
    throw CryptoException(
        CryptoException::ExceptionType::UNSUCCESSFUL_DECODE_EXCEPTION);
}