#pragma once

#include <cstdint>
#include <cassert>

//sign extend
static inline int32_t SE(uint32_t data, int sign) {
    int32_t new_data = (int32_t) data;
    new_data <<= 32 - sign;
    new_data >>= 32 - sign;
    return new_data;
}

// Extract bits
static inline uint32_t EB(uint32_t data, int hi, int lo) {
    assert(hi > lo);
    int offset = 32 - hi;
    return (data << offset) >> (offset + lo);
}

// Check if n is a power of 2
static inline bool isPowerOfTwo(uint32_t n) {
    return (n & (n - 1)) == 0;
}

