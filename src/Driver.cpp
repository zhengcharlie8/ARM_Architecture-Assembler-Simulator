// Class for running simulation

#include "Cpu.h"

#include <cstdio>

int
main(int argc, char** argv) {
    FILE* file = fopen("test", "rb");

    // get file size in instruction words
    fseek(file, 0L, SEEK_END);
    int sz = ftell(file);
    fseek(file, 0L, SEEK_SET);

    int sz_in_words = sz / sizeof(uint32_t);

    // read that many words from the file
    vector<uint32_t> program(sz_in_words);
    fread(program.data(), sizeof(uint32_t), sz_in_words, file);

    fclose(file);

    CPU cpu(program);
    cpu.run();

    printf("num cycles = %d\n", cpu.cycles);
}
