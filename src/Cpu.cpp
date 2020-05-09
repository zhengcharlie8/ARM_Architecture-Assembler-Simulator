#include "Cpu.h"
#include "Util.h"
//
// #include <unistd.h>
#include <cstdio>
#include <cassert>
#include <iostream>


CPU::CPU(const vector<uint32_t>& program) {
    Reg defaultReg;
    defaultReg.setType(Reg::Type::WIDTH32);
    defaultReg.setData(0);

    for (int i = 0; i < 32; i++) {
        reg[i] = defaultReg;
    }
    cycles = 0;
    toEnd = 0;
    residue = program;
    vector<int> temp(5,-1);
    pipeline = temp;
    mem.loadProgram(program);
}
void CPU::reset() {
    Reg defaultReg;
    defaultReg.setType(Reg::Type::WIDTH32);
    defaultReg.setData(0);

    for (int i = 0; i < 32; i++) {
        reg[i] = defaultReg;
    }
    cycles = 0;
    vector<int> temp(5,-1);
    pipeline = temp;
    toEnd = 0;
    mem.loadProgram(residue);
}

void
CPU::run() {
    while (!isHalted) {
        step();
  }
  view();
  printPipe();
}
void
CPU::printPipe()
{
if(isHalted){
  vector<int> temp(5,-1);
  pipeline = temp;
}
printf("\nPipeline:\n");
printf("Fetch:%d\n",pipeline[0]);
printf("Decode:%d\n",pipeline[1]);
printf("Execute:%d\n",pipeline[2]);
printf("Memory Access:%d\n",pipeline[3]);
printf("Write back:%d\n\n",pipeline[4]);
}
void
CPU::addPipe(int des) {
  pipeline[4]=pipeline[3];
    pipeline[3]=pipeline[2];
      pipeline[2]=pipeline[1];
        pipeline[1]=pipeline[0];
          pipeline[0]=des;
}
void
CPU::step() {
    uint32_t instruction = fetch();
    int choice;
    if(toEnd == 1)
    {
      execute(instruction);
      cycles += 10;
      cout<<"cycle: "<<cycles<<endl;
      return;
    }
    while(toEnd == 0){
    cout<<"Press 1 to step\n"<<"press 2 to view registers\n"<<"pres 3 to view pipeline\n"<<"Press 4 to step to end\n"<<"press 5 to restart"<<endl;
    cin>>choice;
    if(choice == 1){
    execute(instruction);
    instruction = fetch();
    cycles += 10;
    cout<<"cycle: "<<cycles<<endl;
  }
    else if(choice == 2)
    view();
    else if(choice == 3)
    printPipe();
    else if(choice ==4)
    {
      execute(instruction);
      cycles += 10;
      cout<<"cycle: "<<cycles<<endl;
      toEnd =10;
      break;
    }
    else if(choice ==4)
      {
        reset();
        break;
      }
  }
}
void
CPU::view(){
  for(int i = 0; i < 12 ;i++) {
printf("R%d:0x%08X R%d:0x%08X\n", i, reg[i].getData(), i+16, reg[i+16].getData());
  }

printf("R%d:0x%08X LR:0x%08X\n", 12, reg[12].getData(), reg[28].getData());
printf("R%d:0x%08X SP:0x%08X\n", 13, reg[13].getData(), reg[29].getData());
printf("R%d:0x%08X APSR:0x%08X\n", 14, reg[14].getData(), reg[30].getData());
printf("R%d:0x%08X PC:0x%08X\n", 15, reg[15].getData(), reg[31].getData());

}

uint32_t
CPU::fetch() {
    // Get address of instruction
    uint32_t address = PC.getData();

    // Increment PC
    PC.setData(address + sizeof(uint32_t));

    // Fetch instruction from memory
    return mem.read(address, &cycles);
}

void
CPU::execute(const uint32_t instruction) {
    // INSTRUCTION TYPES
    const static auto EXECUTION = 0;
    const static auto LOADSTORE = 1;
    const static auto BRANCH    = 2;
    const static auto SPECIAL   = 3;

    const int type = EB(instruction, 32, 30);

    switch (type) {
        case EXECUTION : {
            // EXECUTION INSTRUCTION TYPES
            const static auto OP_ADD  = 0b00000;
            const static auto OP_ADDI = 0b00001;
            const static auto OP_SUB  = 0b00010;
            const static auto OP_SUBI = 0b00011;
            const static auto OP_RSUB = 0b00100;
            const static auto OP_CMP  = 0b00101;
            const static auto OP_CMPI = 0b00110;
            const static auto OP_AND  = 0b00111;
            const static auto OP_ANDI = 0b01000;
            const static auto OP_OR   = 0b01001;
            const static auto OP_ORI  = 0b01010;
            const static auto OP_NOT  = 0b01011;
            const static auto OP_XOR  = 0b01100;
            const static auto OP_XORI = 0b01101;
            const static auto OP_LSR  = 0b01110;
            const static auto OP_LSRI = 0b01111;
            const static auto OP_LSL  = 0b10000;
            const static auto OP_LSLI = 0b10001;
            const static auto OP_ASR  = 0b10010;
            const static auto OP_ROR  = 0b10011;
            const static auto OP_ROL  = 0b10100;
            const static auto OP_MUL  = 0b10101;
            const static auto OP_UMUL = 0b10110;
            const static auto OP_DIV  = 0b10111;
            const static auto OP_UDIV = 0b11000;
            const static auto OP_MOD  = 0b11001;
            const static auto OP_UMOD = 0b11010;
            const static auto OP_MOV  = 0b11011;
            const static auto OP_MOVI = 0b11100;
            const static auto OP_TRN  = 0b11101;

            const auto op = EB(instruction, 30, 25);

            switch (op) {
                case OP_ADD : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    addPipe(dst);
                    printf("ADD r%d r%d r%d\n", dst, op1, op2);
                    ADD(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_ADDI : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t imm = EB(instruction, 15, 0);
                    addPipe(dst);
                    printf("ADD r%d r%d imm=%d\n", dst, op1, imm);
                    ADD(reg[dst], reg[op1], imm);
                    break;
                }
                case OP_SUB : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    addPipe(dst);
                    printf("SUB r%d r%d r%d\n", dst, op1, op2);
                    SUB(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_SUBI : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t imm = EB(instruction, 15, 0);
                    addPipe(dst);
                    printf("SUB r%d r%d imm=%d\n", dst, op1, imm);
                    SUB(reg[dst], reg[op1], imm);
                    break;
                }
                case OP_RSUB : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t imm = EB(instruction, 15, 0);
                    addPipe(dst);
                    printf("RSUB r%d imm=%d r%d\n", dst, imm, op1);
                    RSUB(reg[dst], imm, reg[op1]);
                    break;
                }
                case OP_CMP : {
                    const uint32_t op1 = EB(instruction, 25, 20);
                    const uint32_t op2 = EB(instruction, 20, 15);
                    printf("CMP r%d r%d\n", op1, op2);
                    CMP(reg[op1], reg[op2]);
                    break;
                }
                case OP_CMPI : {
                    const uint32_t op1 = EB(instruction, 25, 20);
                    const int32_t imm = SE(EB(instruction, 20, 0), 20);
                    printf("CMP r%d imm=%d\n", op1, imm);
                    CMP(reg[op1], imm);
                    break;
                }
                case OP_AND : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    addPipe(dst);
                    printf("AND r%d r%d r%d\n", dst, op1, op2);
                    AND(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_ANDI : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t imm = EB(instruction, 15, 0);
                    addPipe(dst);
                    printf("AND r%d r%d imm=%d\n", dst, op1, imm);
                    AND(reg[dst], reg[op1], imm);
                    break;
                }
                case OP_OR : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    addPipe(dst);
                    printf("OR r%d r%d r%d\n", dst, op1, op2);
                    OR(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_ORI : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t imm = EB(instruction, 15, 0);
                    printf("OR r%d r%d imm=%d\n", dst, op1, imm);
                    addPipe(dst);
                    OR(reg[dst], reg[op1], imm);
                    break;
                }
                case OP_NOT : {
                    const uint32_t op1 = EB(instruction, 25, 20);
                    const uint32_t op2 = EB(instruction, 20, 15);
                    printf("NOT r%d r%d\n", op1, op2);
                    NOT(reg[op1], reg[op2]);
                    break;
                }
                case OP_XOR : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    addPipe(dst);
                    printf("XOR r%d r%d r%d\n", dst, op1, op2);
                    XOR(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_XORI : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t imm = EB(instruction, 15, 0);
                    printf("XOR r%d r%d imm=%d\n", dst, op1, imm);
                    addPipe(dst);
                    XOR(reg[dst], reg[op1], imm);
                    break;
                }
                case OP_LSR : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    addPipe(dst);
                    printf("LSR r%d r%d r%d\n", dst, op1, op2);
                    LSR(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_LSRI : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t imm = EB(instruction, 15, 0);
                    addPipe(dst);
                    printf("LSR r%d r%d imm=%d\n", dst, op1, imm);
                    LSR(reg[dst], reg[op1], imm);
                    break;
                }
                case OP_LSL : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    addPipe(dst);
                    printf("LSL r%d r%d r%d\n", dst, op1, op2);
                    LSL(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_LSLI : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t imm = EB(instruction, 15, 0);
                    addPipe(dst);
                    printf("LSL r%d r%d imm=%d\n", dst, op1, imm);
                    LSL(reg[dst], reg[op1], imm);
                    break;
                }
                case OP_ASR : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    addPipe(dst);
                    printf("ASR r%d r%d r%d\n", dst, op1, op2);
                    ASR(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_ROR : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    addPipe(dst);
                    printf("ROR r%d r%d r%d\n", dst, op1, op2);
                    ROR(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_ROL : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    addPipe(dst);
                    printf("ROL r%d r%d r%d\n", dst, op1, op2);
                    ROL(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_MUL : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    addPipe(dst);
                    printf("MUL r%d r%d r%d\n", dst, op1, op2);
                    MUL(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_UMUL : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    addPipe(dst);
                    printf("UMUL r%d r%d r%d\n", dst, op1, op2);
                    UMUL(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_DIV : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    printf("DIV r%d r%d r%d\n", dst, op1, op2);
                    DIV(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_UDIV : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    printf("UDIV r%d r%d r%d\n", dst, op1, op2);
                    UDIV(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_MOD : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    printf("MOD r%d r%d r%d\n", dst, op1, op2);
                    MOD(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_UMOD : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t op1 = EB(instruction, 20, 15);
                    const uint32_t op2 = EB(instruction, 15, 10);
                    printf("UMOD r%d r%d r%d\n", dst, op1, op2);
                    UMOD(reg[dst], reg[op1], reg[op2]);
                    break;
                }
                case OP_MOV : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t src = EB(instruction, 20, 15);
                    printf("MOV r%d r%d\n", dst, src);
                    MOV(reg[dst], reg[src]);
                    break;
                }
                case OP_MOVI : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t imm = EB(instruction, 20, 0);
                    addPipe(dst);
                    printf("MOV r%d imm=%d\n", dst, imm);
                    MOV(reg[dst], imm);
                    break;
                }
                case OP_TRN : {
                    const uint32_t dst = EB(instruction, 25, 20);
                    const uint32_t width = EB(instruction, 20, 18);
                    printf("TRN r%d size=%d\n", dst, width);
                    TRN(reg[dst], (Reg::Type)width);
                    break;
                }
            }

            break;
        }
        case LOADSTORE : {
            // LOADSTORE INSTRUCTION TYPES
            const static auto OP_LDR = 0b0;
            const static auto OP_STR = 0b1;

            const auto op = EB(instruction, 30, 29);

            switch (op) {
                case OP_LDR: {
                    const uint32_t width = EB(instruction, 29, 27);
                    const uint32_t dst = EB(instruction, 27, 22);
                    const uint32_t src = EB(instruction, 22, 17);
                    addPipe(dst);
                    printf("ldr size=%d r%d r%d\n", width, dst, src);
                    LDR(reg[dst], reg[src], (Reg::Type)width);
                    break;
                }
                case OP_STR: {
                    const uint32_t src = EB(instruction, 29, 24);
                    const uint32_t dst = EB(instruction, 24, 19);
                    printf("str r%d r%d\n", src, dst);
                    STR(reg[dst], reg[src]);
                    break;
                }
            }

            break;
        }
        case BRANCH : {
            // BRANCH INSTRUCTION TYPES
            const static auto OP_B    = 0b00;
            const static auto OP_BI   = 0b01;
            const static auto OP_BL   = 0b10;
            const static auto OP_CALL = 0b11;

            const auto op = EB(instruction, 30, 28);

            //TODO: check label sign extension
            switch (op) {
                case OP_B: {
                    const uint32_t cond = EB(instruction, 28, 20);
                    int32_t label = SE(EB(instruction, 24, 0), 20);
                    printf("B cond=%d %d\n", cond, label);
                    B(cond, label);
                    break;
                }
                case OP_BI: {
                    const uint32_t cond = EB(instruction, 28, 20);
                    const uint32_t idx  = EB(instruction, 20, 15);
                    printf("BI cond=%d r%d\n", cond, idx);
                    BI(cond, reg[idx]);
                    break;
                }
                case OP_BL: {
                    int32_t label = SE(EB(instruction, 28, 0), 28);
                    printf("BL %d\n", label);
                    BL(label);
                    break;
                }
                case OP_CALL: {
                    const uint32_t idx = EB(instruction, 28, 23);
                    printf("CALL r%d\n", idx);
                    CALL(reg[idx]);
                    break;
                }
            }

            break;
        }
        case SPECIAL: {
            printf("hlt\n");
            HLT();
            break;
        }
    }
}

/******* EXECUTION INSTRUCTIONS *******/

void
CPU::ADD(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    int width = dst.width();

    uint32_t data;
    switch(width) {
        case 1: {
            uint8_t res;
            if (__builtin_add_overflow((uint8_t)op1_data, (uint8_t)op2_data, &res))
                FLAGS.setData(OF);
            data = res;
            break;
        }
        case 2: {
            uint16_t res;
            if(__builtin_add_overflow((uint16_t)op1_data, (uint16_t)op2_data, &res))
                FLAGS.setData(OF);
            data = res;
            break;
        }
        case 4: {
            uint32_t res;
            if(__builtin_add_overflow((uint32_t)op1_data, (uint32_t)op2_data, &res))
                FLAGS.setData(OF);
            data = res;
            break;
        }
    }

    dst.setData(data);
}

void
CPU::ADD(Reg& dst, Reg& op1, uint32_t imm) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, Reg::Type::WIDTH16);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = imm;

    int width = dst.width();

    uint32_t data;
    switch(width) {
        case 1: {
            uint8_t res;
            if (__builtin_add_overflow((uint8_t)op1_data, (uint8_t)op2_data, &res))
                FLAGS.setData(OF);
            data = res;
            break;
        }
        case 2: {
            uint16_t res;
            if(__builtin_add_overflow((uint16_t)op1_data, (uint16_t)op2_data, &res))
                FLAGS.setData(OF);
            data = res;
            break;
        }
        case 4: {
            uint32_t res;
            if(__builtin_add_overflow((uint32_t)op1_data, (uint32_t)op2_data, &res))
                FLAGS.setData(OF);
            data = res;
            break;
        }
    }

    dst.setData(data);
}

void
CPU::SUB(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    int width = dst.width();

    uint32_t data;
    switch(width) {
        case 1: {
            uint8_t res;
            if (__builtin_sub_overflow((uint8_t)op1_data, (uint8_t)op2_data, &res))
                FLAGS.setData(UF);
            data = res;
            break;
        }
        case 2: {
            uint16_t res;
            if(__builtin_sub_overflow((uint16_t)op1_data, (uint16_t)op2_data, &res))
                FLAGS.setData(UF);
            data = res;
            break;
        }
        case 4: {
            uint32_t res;
            if(__builtin_sub_overflow((uint32_t)op1_data, (uint32_t)op2_data, &res))
                FLAGS.setData(UF);
            data = res;
            break;
        }
    }

    dst.setData(data);
}

void
CPU::SUB(Reg& dst, Reg& op1, uint32_t imm) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, Reg::Type::WIDTH16);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = imm;

    int width = dst.width();

    uint32_t data;
    switch(width) {
        case 1: {
            uint8_t res;
            if (__builtin_sub_overflow((uint8_t)op1_data, (uint8_t)op2_data, &res))
                FLAGS.setData(UF);
            data = res;
            break;
        }
        case 2: {
            uint16_t res;
            if(__builtin_sub_overflow((uint16_t)op1_data, (uint16_t)op2_data, &res))
                FLAGS.setData(UF);
            data = res;
            break;
        }
        case 4: {
            uint32_t res;
            if(__builtin_sub_overflow((uint32_t)op1_data, (uint32_t)op2_data, &res))
                FLAGS.setData(UF);
            data = res;
            break;
        }
    }

    dst.setData(data);
}

void
CPU::RSUB(Reg& dst, uint32_t imm, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op2, Reg::Type::WIDTH16);
    dst.setType(dst_size);

    uint32_t op1_data = imm;
    uint32_t op2_data = op2.getData();

    int width = dst.width();

    uint32_t data;
    switch(width) {
        case 1: {
            uint8_t res;
            if (__builtin_sub_overflow((uint8_t)op1_data, (uint8_t)op2_data, &res))
                FLAGS.setData(UF);
            data = res;
            break;
        }
        case 2: {
            uint16_t res;
            if(__builtin_sub_overflow((uint16_t)op1_data, (uint16_t)op2_data, &res))
                FLAGS.setData(UF);
            data = res;
            break;
        }
        case 4: {
            uint32_t res;
            if(__builtin_sub_overflow((uint32_t)op1_data, (uint32_t)op2_data, &res))
                FLAGS.setData(UF);
            data = res;
            break;
        }
    }

    dst.setData(data);
}

void
CPU::CMP(Reg& arg1, Reg& arg2) {
    uint32_t a1_data = arg1.getData();
    uint32_t a2_data = arg2.getData();

    //printf("%d %d\n", a1_data, a2_data);

    uint32_t flags = 0;

    if (a1_data > a2_data)  flags |= GT;
    if (a1_data < a2_data)  flags |= LT;
    if (a1_data == a2_data) flags |= EQ;
    if (a1_data != a2_data) flags |= NE;

    FLAGS.setData(flags);
}

void
CPU::CMP(Reg& arg1, int32_t imm) {
    uint32_t a1_data = arg1.getData();
    uint32_t a2_data = (uint32_t)imm;

    uint32_t flags = 0;

    if (a1_data > a2_data)  flags |= GT;
    if (a1_data < a2_data)  flags |= LT;
    if (a1_data == a2_data) flags |= EQ;
    if (a1_data != a2_data) flags |= NE;

    FLAGS.setData(flags);
}

void
CPU::AND(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    dst.setData(op1_data & op2_data);
}

void
CPU::AND(Reg& dst, Reg& op1, uint32_t imm) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, Reg::Type::WIDTH16);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = imm;

    dst.setData(op1_data & op2_data);
}

void
CPU::OR(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    dst.setData(op1_data | op2_data);
}

void
CPU::OR(Reg& dst, Reg& op1, uint32_t imm) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, Reg::Type::WIDTH16);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = imm;

    dst.setData(op1_data | op2_data);
}

void
CPU::NOT(Reg& dst, Reg& arg) {
    dst.setType(arg.getType());
    dst.setData(~arg.getData());
}

void
CPU::XOR(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    dst.setData(op1_data ^ op2_data);
}

void
CPU::XOR(Reg& dst, Reg& op1, uint32_t imm) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, Reg::Type::WIDTH16);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = imm;

    dst.setData(op1_data ^ op2_data);
}

void
CPU::LSR(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    dst.setData(op1_data >> op2_data);
}

void
CPU::LSR(Reg& dst, Reg& op1, uint32_t imm) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, Reg::Type::WIDTH16);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = imm;

    dst.setData(op1_data >> op2_data);
}

void
CPU::LSL(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    dst.setData(op1_data << op2_data);
}

void
CPU::LSL(Reg& dst, Reg& op1, uint32_t imm) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, Reg::Type::WIDTH16);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = imm;

    dst.setData(op1_data << op2_data);
}

void
CPU::ASR(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    int32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    dst.setData(op1_data >> op2_data);
}

void
CPU::ROR(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    int width = dst.width();

    uint32_t data;
    switch(width) {
        case 1: {
            uint8_t op1 = (uint8_t)op1_data;
            uint8_t op2 = (uint8_t)op2_data;
            data = (op1 >> op2 | op1 << (8 - op2));
            break;
        }
        case 2: {
            uint8_t op1 = (uint16_t)op1_data;
            uint8_t op2 = (uint16_t)op2_data;
            data = (op1 >> op2 | op1 << (16 - op2));
            break;
        }
        case 4: {
            uint8_t op1 = (uint32_t)op1_data;
            uint8_t op2 = (uint32_t)op2_data;
            data = (op1 >> op2 | op1 << (32 - op2));
            break;
        }
    }

    dst.setData(data);
}

void
CPU::ROL(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    int width = dst.width();

    uint32_t data;
    switch(width) {
        case 1: {
            uint8_t op1 = (uint8_t)op1_data;
            uint8_t op2 = (uint8_t)op2_data;
            data = (op1 << op2 | op1 >> (8 - op2));
            break;
        }
        case 2: {
            uint8_t op1 = (uint16_t)op1_data;
            uint8_t op2 = (uint16_t)op2_data;
            data = (op1 << op2 | op1 >> (16 - op2));
            break;
        }
        case 4: {
            uint8_t op1 = (uint32_t)op1_data;
            uint8_t op2 = (uint32_t)op2_data;
            data = (op1 << op2 | op1 >> (32 - op2));
            break;
        }
    }

    dst.setData(data);
}

void
CPU::MUL(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    int width = dst.width();

    uint32_t data;
    switch(width) {
        case 1: {
            int8_t res;
            if (__builtin_mul_overflow((int8_t)op1_data, (int8_t)op2_data, &res))
                FLAGS.setData(OF);
            data = res;
            break;
        }
        case 2: {
            int16_t res;
            if(__builtin_mul_overflow((int16_t)op1_data, (int16_t)op2_data, &res))
                FLAGS.setData(OF);
            data = res;
            break;
        }
        case 4: {
            int32_t res;
            if(__builtin_mul_overflow((int32_t)op1_data, (int32_t)op2_data, &res))
                FLAGS.setData(OF);
            data = res;
            break;
        }
    }

    dst.setData(data);
}

void
CPU::UMUL(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    int width = dst.width();

    uint32_t data;
    switch(width) {
        case 1: {
            uint8_t res;
            if (__builtin_mul_overflow((uint8_t)op1_data, (uint8_t)op2_data, &res))
                FLAGS.setData(OF);
            data = res;
            break;
        }
        case 2: {
            uint16_t res;
            if(__builtin_mul_overflow((uint16_t)op1_data, (uint16_t)op2_data, &res))
                FLAGS.setData(OF);
            data = res;
            break;
        }
        case 4: {
            uint32_t res;
            if(__builtin_mul_overflow((uint32_t)op1_data, (uint32_t)op2_data, &res))
                FLAGS.setData(OF);
            data = res;
            break;
        }
    }

    dst.setData(data);
}

void
CPU::DIV(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    if (op2_data == 0) {
        FLAGS.setData(DZ);
    } else {
        int width = dst.width();

        uint32_t data;
        switch(width) {
            case 1: {
                data = (int8_t)op1_data / (int8_t)op2_data;
                break;
            }
            case 2: {
                data = (int16_t)op1_data / (int16_t)op2_data;
                break;
            }
            case 4: {
                data = (int32_t)op1_data / (int32_t)op2_data;
                break;
            }
        }

        dst.setData(data);
    }
}

void
CPU::UDIV(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    if (op2_data == 0) {
        FLAGS.setData(DZ);
    } else {
        int width = dst.width();

        uint32_t data;
        switch(width) {
            case 1: {
                data = (uint8_t)op1_data / (uint8_t)op2_data;
                break;
            }
            case 2: {
                data = (uint16_t)op1_data / (uint16_t)op2_data;
                break;
            }
            case 4: {
                data = (uint32_t)op1_data / (uint32_t)op2_data;
                break;
            }
        }

        dst.setData(data);
    }
}

void
CPU::MOD(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    if (op2_data == 0) {
        FLAGS.setData(DZ);
    } else {
        int width = dst.width();

        uint32_t data;
        switch(width) {
            case 1: {
                data = (int8_t)op1_data % (int8_t)op2_data;
                break;
            }
            case 2: {
                data = (int16_t)op1_data % (int16_t)op2_data;
                break;
            }
            case 4: {
                data = (int32_t)op1_data % (int32_t)op2_data;
                break;
            }
        }

        dst.setData(data);
    }
}

void
CPU::UMOD(Reg& dst, Reg& op1, Reg& op2) {
    Reg::Type dst_size = Reg::chooseMaxWidth(op1, op2);
    dst.setType(dst_size);

    uint32_t op1_data = op1.getData();
    uint32_t op2_data = op2.getData();

    if (op2_data == 0) {
        FLAGS.setData(DZ);
    } else {
        int width = dst.width();

        uint32_t data;
        switch(width) {
            case 1: {
                data = (uint8_t)op1_data % (uint8_t)op2_data;
                break;
            }
            case 2: {
                data = (uint16_t)op1_data % (uint16_t)op2_data;
                break;
            }
            case 4: {
                data = (uint32_t)op1_data % (uint32_t)op2_data;
                break;
            }
        }

        dst.setData(data);
    }
}

void
CPU::MOV(Reg& dst, uint32_t imm) {
    dst.setType(Reg::Type::WIDTH32);
    dst.setData(imm);
}

void
CPU::MOV(Reg& dst, Reg& src) {
    dst.setType(src.getType());
    dst.setData(src.getData());
}

void
CPU::TRN(Reg& arg, Reg::Type type) {
    uint32_t data = arg.getData();
    arg.setType(type);
    arg.setData(data);
}

/******* LOAD/STORE INSTRUCTIONS *******/

void
CPU::LDR(Reg& dst, Reg& src, Reg::Type type) {
    assert(type != Reg::Type::UNUSED);

    const uint32_t address = src.getData();
    uint32_t data = mem.read(address, &cycles);

    dst.setType(type);
    dst.setData(data);
}

void
CPU::STR(Reg& dst, Reg& src) {
    const uint32_t address = dst.getData();
    const uint32_t data = src.getData();

    uint32_t curr = mem.read(address, &cycles);
    uint64_t mask = (1 << (8*src.width())) - 1;
    curr &= (uint32_t) mask;
    curr |= data;

    mem.write(address, data, &cycles);
}

/******* LOAD/STORE INSTRUCTIONS *******/

void
CPU::B(uint32_t cond, int32_t label) {
    if ((FLAGS.getData() & cond) == cond) {
        PC.setData(PC.getData() + label);
    }
}

void
CPU::BI(uint32_t cond, Reg& arg) {
    if ((FLAGS.getData() & cond) == cond) {
        PC.setData(arg.getData());
    }
}

void
CPU::BL(int32_t label) {
    uint32_t pc = PC.getData();

    LR.setType(Reg::Type::WIDTH32);
    LR.setData(pc);

    PC.setData(pc + label);
}

void
CPU::CALL(Reg& arg) {
    uint32_t pc = PC.getData();

    LR.setType(Reg::Type::WIDTH32);
    LR.setData(pc);

    PC.setData(arg.getData());
}

/******* SPECIAL INSTRUCTIONS *******/

void
CPU::HLT() {
    isHalted = true;
}
