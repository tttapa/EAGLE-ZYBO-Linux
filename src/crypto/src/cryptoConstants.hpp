#pragma once

// Number of times to run the decrypt function before
// throwing a timeout exception.
const uint8_t nbTrials = 3;

// Path to the drone master key on ZYBO.
const std::string dmKeyPath("/media/droneMasterKey0.bin");

// The number of bytes in the drone master key.
const uint16_t dmKeySizeInBytes = 128 / 8;