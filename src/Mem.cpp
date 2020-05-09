#include "Mem.h"

#include <cstdio>

#ifdef CACHE
void
Cache::invalidate(uint32_t address) {
    const int idx = INDEX(address);
    const int tag = TAG(address);

    if (lines[idx].tag == tag) {
        lines[idx].valid = false;
    }
}

bool
Cache::read(uint32_t address, Line* line) {
    const uint32_t idx = INDEX(address);
    const uint32_t tag = TAG(address);

//    printf("index = %d\n", idx);
//    printf("tag   = %d\n", tag);
//    Cache::Line l = lines[idx];
//    printf("cache : %x  %x  %x  %x\n", l.line[0], l.line[1], l.line[2], l.line[3]);

    if (lines[idx].tag == tag && lines[idx].valid) {
        *line = lines[idx];
        return true;
    } else {
        return false;
    }
}

void
Cache::write(uint32_t address, Line line) {
    const uint32_t idx = INDEX(address);
    lines[idx] = line;

//    Cache::Line l = lines[idx];
//    printf("line  : %x  %x  %x  %x\n", line.line[0], line.line[1], line.line[2], line.line[3]);
//    printf("cache : %x  %x  %x  %x\n", l.line[0], l.line[1], l.line[2], l.line[3]);
}
#endif

void
Memory::loadProgram(const vector<uint32_t>& program) {
    // load program starting at address 0
    for (int i = 0; i < program.size(); i++) {
        ram[i] = program[i];
    }
}

uint32_t
Memory::read(uint32_t address, uint32_t* cycles) {
    // Unaligned read
    assert(address % sizeof(uint32_t) == 0);
    address /= 4;

#ifdef CACHE
    Cache::Line line;

    bool hit = false;

    int i;
    for (i = 0; i < caches.size(); i++) {
        Cache& cache = caches[i];
        *cycles += cache.DELAY;
        hit = cache.read(address, &line);

        if (hit) {
            break;
        }
    }

    // Read line ram if not in cache
    if (!hit) {
        *cycles += RAM_DELAY;
        for (int word = 0; word < WORDS_PER_LINE; word++) {
            line.line[word] = ram[address + word];
        }

        line.valid = true;
    }

    // Writeback into caches
    for (i--; i >= 0; i--) {
        Cache& cache = caches[i];
        *cycles += cache.DELAY;

        line.tag = cache.TAG(address);
        cache.write(address, line);
    }

    return line.line[Cache::Line::OFFSET(address)];
#else
    *cycles += RAM_DELAY;
    return ram[address];
#endif
}

void
Memory::write(uint32_t address, uint32_t data, uint32_t* cycles) {
    // Unaligned write
    assert(address % sizeof(uint32_t) == 0);

    address /= 4;

#ifdef CACHE
    // writethrough - no allocate
    for (auto& cache : caches) {
        *cycles += cache.DELAY;
        cache.invalidate(address);
    }
#endif

    *cycles += RAM_DELAY;
    ram[address] = data;
}

