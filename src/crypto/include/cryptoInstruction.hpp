#pragma once

#include <cstdint>
#include <vector>

class CryptoInstruction {

  public:
    enum InstructionType { GOTO, LAND, UNKNOWN };

  private:
    InstructionType instructionType;
    uint8_t x;
    uint8_t y;
    std::vector<uint8_t> unknownData;

  public:
    CryptoInstruction(InstructionType instructionType, uint8_t x = 0,
                      uint8_t y = 0)
        : instructionType(instructionType), x(x), y(y) {}

    CryptoInstruction(InstructionType instructionType,
                      const std::vector<uint8_t> &unknownData)
        : instructionType(instructionType), unknownData(unknownData) {}

    InstructionType getInstructionType() const { return instructionType; }
    uint8_t getXCoordinate() const { return x; }
    uint8_t getYCoordinate() const { return y; }
    const std::vector<uint8_t>& getUnknownData() const { return unknownData; }
};