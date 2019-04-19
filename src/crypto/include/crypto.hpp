#include <cstdint>  // uint32_t

/** 
 * @brief   Write the given word to Crypto FPGA register R0.
 * 
 * @param   value
 *          The word to write to R0.
 */
void writeR0(uint32_t value);

/**
 * @brief   Read the value of Crypto FPGA register R0.
 * 
 * @return  The value of R0.
 */
uint32_t readR0();

/**
 * @brief   Read the value of Crypto FPGA register R1.
 * 
 * @return  The value of R1.
 */
uint32_t readR1();