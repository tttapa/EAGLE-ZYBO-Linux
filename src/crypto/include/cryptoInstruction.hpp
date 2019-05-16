#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

class CryptoInstruction {

  public:
    enum class InstructionType { GOTO, LAND, UNKNOWN };

    struct GOTO_t {
    } static GOTO;
    struct LAND_t {
    } static LAND;
    struct UNKNOWN_t {
    } static UNKNOWN;

  private:
    InstructionType instructionType;
    uint8_t nextX;
    uint8_t nextY;
    std::vector<uint8_t> unknownData;

    uint8_t currentX;
    uint8_t currentY;

  public:
    CryptoInstruction(GOTO_t, uint8_t nextX, uint8_t nextY, uint8_t currentX,
                      uint8_t currentY)
        : instructionType(InstructionType::GOTO), nextX(nextX), nextY(nextY),
          currentX(currentX), currentY(currentY) {}

    CryptoInstruction(LAND_t, uint8_t currentX, uint8_t currentY)
        : instructionType(InstructionType::LAND), currentX(currentX),
          currentY(currentY) {}

    CryptoInstruction(UNKNOWN_t, const std::vector<uint8_t> &unknownData,
                      uint8_t currentX, uint8_t currentY)
        : instructionType(InstructionType::UNKNOWN), unknownData(unknownData),
          currentX(currentX), currentY(currentY) {}

    InstructionType getInstructionType() const { return instructionType; }

    uint8_t getXCoordinate() const {
        if (getInstructionType() != InstructionType::GOTO)
            throw std::logic_error(
                "Error: only GOTO instructions have a valid x-coordinate");
        return nextX;
    }

    uint8_t getYCoordinate() const {
        if (getInstructionType() != InstructionType::GOTO)
            throw std::logic_error(
                "Error: only GOTO instructions have a valid y-coordinate");
        return nextY;
    }

    const std::vector<uint8_t> &getUnknownData() const {
        if (getInstructionType() != InstructionType::UNKNOWN)
            throw std::logic_error(
                "Error: only UNKNOWN instructions have valid unknown data");
        return unknownData;
    }

    uint8_t getCurrentXCoordinate() const { return currentX; }
    uint8_t getCurrentYCoordinate() const { return currentY; }
};