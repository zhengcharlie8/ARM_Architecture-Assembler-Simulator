#pragma once

#include "Util.h"

#include <vector>

#include <cstdint>
#include <cmath>

using namespace std;

// RAM CONFIGURATION
const auto RAM_SIZE  = 0x2000/sizeof(uint32_t);
const auto RAM_DELAY = 100;

#define CACHE

#ifdef CACHE
// CACHE CONFIGURATION
const auto WORDS_PER_LINE = 4;
const auto LOG2_WORDS_PER_LINE = 2;

class Cache
{
    public:
        struct Line {
            Line() : valid(false) {}

            bool valid;
            uint32_t tag;
            uint32_t line[WORDS_PER_LINE];

            static uint32_t OFFSET(uint32_t address) {
                return EB(address, OFFSET_BITS_HI, OFFSET_BITS_LO);
            }

            static const auto OFFSET_BITS_LO = 0;
            static const auto OFFSET_BITS_HI = OFFSET_BITS_LO + LOG2_WORDS_PER_LINE;
        };

        Cache(int numLines, int delay) :
            INDEX_BITS_LO(Line::OFFSET_BITS_HI),
            INDEX_BITS_HI(INDEX_BITS_LO + log2(numLines)),
            TAG_BITS_LO(INDEX_BITS_HI),
            TAG_BITS_HI(32),
            DELAY(delay),
            lines(numLines)
        {
            assert(isPowerOfTwo(WORDS_PER_LINE));
            assert(isPowerOfTwo(numLines));
        }

        void invalidate(uint32_t address);
        bool read(uint32_t address, Line* line);
        void write(uint32_t address, Line line);

        uint32_t INDEX(uint32_t address) {
            return EB(address, INDEX_BITS_HI, INDEX_BITS_LO);
        }

        uint32_t TAG(uint32_t address) {
            return EB(address, TAG_BITS_HI, TAG_BITS_LO);
        }

        const uint32_t DELAY;

    private:
        const int INDEX_BITS_LO;
        const int INDEX_BITS_HI;
        const int TAG_BITS_LO;
        const int TAG_BITS_HI;

        vector<Line> lines;
};
#endif

class Memory
{
    public:
        void loadProgram(const vector<uint32_t>& program);

        uint32_t read(uint32_t address, uint32_t* cycles);
        void write(uint32_t address, uint32_t data, uint32_t* cycles);

        // RAM
        vector<uint32_t> ram = vector<uint32_t>(RAM_SIZE);

#ifdef CACHE
        // CACHE HIERARCHY
        vector<Cache> caches = {
            Cache(128, 2), // L1: 128 lines, 2 cycle delay
        };
#endif
};

