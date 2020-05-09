#pragma once

#include "Reg.h"
#include "Mem.h"

#include <vector>

#include <cstdint>

using namespace std;

class CPU {
    public:
        CPU(const vector<uint32_t>& program);
        void run();
        void step();
        void stepToEnd();
        void view();
        void reset();
        void addPipe(int dst);
        void printPipe();

        // True if the program has finished running
        vector<int> pipeline;
        bool isHalted = false;
        bool toEnd = false;
        vector<uint32_t> residue;

        // Cycle Counter
        uint32_t cycles;

        // Memory
        Memory mem;

        // Registers
        union {
            struct {
                Reg r0;
                Reg r1;
                Reg r2;
                Reg r3;
                Reg r4;
                Reg r5;
                Reg r6;
                Reg r7;
                Reg r8;
                Reg r9;
                Reg r10;
                Reg r11;
                Reg r12;
                Reg r13;
                Reg r14;
                Reg r15;
                Reg r16;
                Reg r17;
                Reg r18;
                Reg r19;
                Reg r20;
                Reg r21;
                Reg r22;
                Reg r23;
                Reg r24;
                Reg r25;
                Reg r26;
                Reg r27;
                Reg LR;
                Reg SP;
                Reg FLAGS;
                Reg PC;
            };
            Reg reg[32];
        };
    private:
        uint32_t fetch();
        void execute(uint32_t instruction);

        // EXECUTION
        void ADD(Reg& dst, Reg& op1, Reg& op2);
        void ADD(Reg& dst, Reg& op1, uint32_t imm);
        void SUB(Reg& dst, Reg& op1, Reg& op2);
        void SUB(Reg& dst, Reg& op1, uint32_t imm);
        void RSUB(Reg& dst, uint32_t imm, Reg& op2);
        void CMP(Reg& arg1, Reg& arg2);
        void CMP(Reg& arg1, int32_t imm);
        void AND(Reg& dst, Reg& op1, Reg& op2);
        void AND(Reg& dst, Reg& op1, uint32_t imm);
        void OR(Reg& dst, Reg& op1, Reg& op2);
        void OR(Reg& dst, Reg& op1, uint32_t imm);
        void NOT(Reg& dst, Reg& arg);
        void XOR(Reg& dst, Reg& op1, Reg& op2);
        void XOR(Reg& dst, Reg& op1, uint32_t imm);
        void LSR(Reg& dst, Reg& op1, Reg& op2);
        void LSR(Reg& dst, Reg& op1, uint32_t imm);
        void LSL(Reg& dst, Reg& op1, Reg& op2);
        void LSL(Reg& dst, Reg& op1, uint32_t imm);
        void ASR(Reg& dst, Reg& op1, Reg& op2);
        void ROR(Reg& dst, Reg& op1, Reg& op2);
        void ROL(Reg& dst, Reg& op1, Reg& op2);
        void MUL(Reg& dst, Reg& op1, Reg& op2);
        void UMUL(Reg& dst, Reg& op1, Reg& op2);
        void DIV(Reg& dst, Reg& op1, Reg& op2);
        void UDIV(Reg& dst, Reg& op1, Reg& op2);
        void MOD(Reg& dst, Reg& op1, Reg& op2);
        void UMOD(Reg& dst, Reg& op1, Reg& op2);
        void MOV(Reg& dst, uint32_t imm);
        void MOV(Reg& dst, Reg& src);
        void TRN(Reg& arg, Reg::Type type);

        // LOAD/STORE
        void LDR(Reg& dst, Reg& src, Reg::Type type);
        void STR(Reg& dst, Reg& src);

        // BRANCH
        void B(uint32_t cond, int32_t label);
        void BI(uint32_t cond, Reg& arg);
        void BL(int32_t label);
        void CALL(Reg& arg);

        void HLT();

        // FLAGS
        enum COND {
            AL = 0,
            GT = 1 << 0,
            LT = 1 << 1,
            EQ = 1 << 2,
            NE = 1 << 3,
            OF = 1 << 4,
            UF = 1 << 5,
            CA = 1 << 6,
            DZ = 1 << 7,
        };
};
