#pragma once

#include <cryptoException.hpp>
#include <cryptoInstruction.hpp>
#include <implementation.hpp>

/**
 * @brief   Decrypt the given QR code.
 * 
 * @param   qrCode
 *          The QR to decrypt. The QR code should be base64 decoded
 *          already.
 * @param   implementation
 *          The implementation to use can either be the software
 *          implementation or the hardware implementation.
 * 
 * @return  CryptoInstruction(CryptoInstruction::GOTO)
 *          This instruction tells the drone to move to the
 *          position (getXCoordinate(), getYCoordinate()).
 * @return  CryptoInstruction(CryptoInstruction::LAND)
 *          This instruction tells the drone to land.
 * @return  CryptoInstruction(CryptoInstruction::UNKNOWN_INSTRUCTION)
 *          The decoded instruction could not be interpreted. The
 *          method getUnknownData() returns a raw array specifying a
 *          decoded instruction that is not yet supported.
 * 
 * @throws  CryptoException(CryptoException::TIMEOUT_EXCEPTION)
 *          There is a problem with the FPGA. For multiple attempts, it took
 *          longer than a certain amount of time specifying the maximum
 *          time needed to decrypt the instruction. You should not run crypto
 *          again.
 * @throws  CryptoException(CryptoException::UNSUCCESSFUL_DECODE_EXCEPTION)
 *          The given QR code could not be decoded. You should run crypto again
 *          with a new picture.
 * @throws  CryptoException(CryptoException::UNKNOWN_ERROR_EXCEPTION)
 *          An unknown error has occurred. You can choose yourself whether
 *          you run crypto again.
 */
CryptoInstruction
decrypt(const std::vector<uint8_t> &qrCode,
        Implementation implementation = Implementation::HWImplementation);