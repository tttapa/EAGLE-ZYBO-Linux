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
    uint8_t x;
    uint8_t y;
    std::vector<uint8_t> unknownData;

  public:
    CryptoInstruction(GOTO_t, uint8_t x, uint8_t y)
        : instructionType(InstructionType::GOTO), x(x), y(y) {}

    CryptoInstruction(LAND_t) : instructionType(InstructionType::LAND) {}

    CryptoInstruction(UNKNOWN_t, const std::vector<uint8_t> &unknownData)
        : instructionType(InstructionType::UNKNOWN), unknownData(unknownData) {}

    InstructionType getInstructionType() const { return instructionType; }
    uint8_t getXCoordinate() const {
        if (getInstructionType() != InstructionType::GOTO)
            throw std::logic_error(
                "Error: Only GOTO instructions have a valid x-coordinate");
        return x;
    }
    uint8_t getYCoordinate() const {
        if (getInstructionType() != InstructionType::GOTO)
            throw std::logic_error(
                "Error: Only GOTO instructions have a valid y-coordinate");
        return y;
    }
    const std::vector<uint8_t> &getUnknownData() const {
        if (getInstructionType() != InstructionType::GOTO)
            throw std::logic_error(
                "Error: Only UNKNOWN instructions have valid unknown data");
        return unknownData;
    }
};