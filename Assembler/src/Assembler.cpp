#include "Assembler.h"

#include <regex>

#include <cassert>
#include <cstdio>

Assembler::Assembler(const char* fname_in, const char* fname_out) :
    line_num(0)
{
    file_in.open(fname_in);
    if (!file_in.is_open()) {
        cout << "Error: failed to open input file '" << fname_in << "'" << endl;
        exit(-1);
    }

    file_out.open(fname_out, ofstream::binary);
    if (!file_in.is_open()) {
        cout << "Error: failed to open output file '" << fname_out << "'" << endl;
        exit(-1);
    }
}

Assembler::~Assembler() {
    file_in.close();
    file_out.close();
}

void Assembler::parse() {
    int num_lines = 0;
    // parse labels
    {
        int valid_lines = 0;

	    string line;
	    while(getline(file_in, line)) {
            num_lines += 1;
            if (line.size() == 0)
                continue;

            int label_end = line.find(":");

            if (label_end == string::npos) {
                valid_lines += 1;
                continue;
            }

		    string label = line.substr(0, label_end);
            labels[label] = valid_lines;
        }
    }

    // Go back to start of file
    file_in.clear();
    file_in.seekg(0, ios::beg);

    string line;
    for (int i = 0; i < num_lines-1; i++) {
	    getline(file_in, line);

        line_num += 1;

        const uint32_t instruction = parse_line(line);

        if (instruction == -1)
            continue;

        program.emplace_back(instruction);
    }

	getline(file_in, line);
    int data_loc_start = line.find_first_not_of("{", 4);
    int data_loc_end   = line.find_first_not_of("0123456789", data_loc_start);

    int data_loc = stoi(line.substr(data_loc_start, data_loc_end)) / 4;

    for (int i = program.size(); i < data_loc; i++) {
        program.emplace_back(0);
    }

    int start = 0;
    int end = data_loc_end;
    while(true) {
        start = line.find_first_of("0123456789", end);
        if (start == string::npos) break;
        end = line.find_first_not_of("0123456789", start);

        string val = line.substr(start, end-1);
        program.emplace_back(stoi(val));
    }
}

void Assembler::write() {
    file_out.write((char*)&program[0], program.size() * sizeof(uint32_t));
}

uint32_t Assembler::parse_line(string& line)
{
    if (line.size() == 0)
        return -1;

	string arg[4];
	size_t start = line.find_first_not_of(" \t");
	size_t end = 0;
	int i = 0;
	while(start != string::npos){
		end = line.find(" ", start);
		arg[i] = line.substr(start, end - start);
		start = line.find_first_not_of(" ", end);

        // Allow comments at the end of lines
        if (line[start] == '/') {
            break;
        }
		i++;
	}

    if (arg[0].back() == ':')
        return -1;

    switch (INSTRUCTIONS[arg[0]]) {
        case ADD: {
            const uint32_t op  = isReg(arg[3]) ? 0b00000 : 0b00001;
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = isReg(arg[3]) ?
                parse_reg(arg[3])     << 10 : // reg
                parse_imm(arg[3], 15) <<  0 ; // imm

            return 0b00 << 30 | // TYPE
                     op << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 <<  0;  // OP2
        }

        case SUB: {
            const uint32_t op  = isReg(arg[3]) ? 0b00010 : 0b00011;
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = isReg(arg[3]) ?
                parse_reg(arg[3])     << 10 : // reg
                parse_imm(arg[3], 15) <<  0 ; // imm

            return 0b00 << 30 | // TYPE
                     op << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 <<  0;  // OP2
        }

        case RSUB: {
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = parse_imm(arg[3], 15);

            return    0b00 << 30 | // TYPE
                   0b00100 << 25 | // OP
                       dst << 20 | // DST
                       op1 << 15 | // OP1
                       op2 <<  0;  // OP2
        }

        case CMP: {
            const uint32_t op  = isReg(arg[2]) ? 0b00101 : 0b00110;
            const uint32_t op1 = parse_reg(arg[1]);
            const uint32_t op2 = isReg(arg[2]) ?
                parse_reg(arg[2])     << 15 : // reg
                parse_imm(arg[2], 20) <<  0 ; // imm

            return 0b00 << 30 | // TYPE
                     op << 25 | // OP
                    op1 << 20 | // OP1
                    op2 <<  0;  // OP2
        }

        case AND: {
            const uint32_t op  = isReg(arg[3]) ? 0b00111 : 0b01000;
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = isReg(arg[3]) ?
                parse_reg(arg[3])     << 10 : // reg
                parse_imm(arg[3], 15) <<  0 ; // imm

            return 0b00 << 30 | // TYPE
                     op << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 <<  0;  // OP2
        }

        case OR: {
            const uint32_t op  = isReg(arg[3]) ? 0b01001 : 0b01010;
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = isReg(arg[3]) ?
                parse_reg(arg[3])     << 10 : // reg
                parse_imm(arg[3], 15) <<  0 ; // imm

            return 0b00 << 30 | // TYPE
                     op << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 <<  0;  // OP2
        }

        case NOT: {
            const uint32_t op1 = parse_reg(arg[1]);
            const uint32_t op2 = parse_reg(arg[2]);

            return     0b00 << 30 | // TYPE
                    0b01011 << 25 | // OP
                        op1 << 20 | // OP1
                        op2 << 15;  // OP2
        }

        case XOR: {
            const uint32_t op  = isReg(arg[3]) ? 0b01100 : 0b01101;
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = isReg(arg[3]) ?
                parse_reg(arg[3])     << 10 : // reg
                parse_imm(arg[3], 15) <<  0 ; // imm

            return 0b00 << 30 | // TYPE
                     op << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 <<  0;  // OP2
        }

        case LSR: {
            const uint32_t op  = isReg(arg[3]) ? 0b01110 : 0b01111;
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = isReg(arg[3]) ?
                parse_reg(arg[3])     << 10 : // reg
                parse_imm(arg[3], 15) <<  0 ; // imm

            return 0b00 << 30 | // TYPE
                     op << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 <<  0;  // OP2
        }

        case LSL: {
            const uint32_t op  = isReg(arg[3]) ? 0b10000 : 0b10001;
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = isReg(arg[3]) ?
                parse_reg(arg[3])     << 10 : // reg
                parse_imm(arg[3], 15) <<  0 ; // imm

            return 0b00 << 30 | // TYPE
                     op << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 <<  0;  // OP2
        }

        case ASR: {
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = parse_reg(arg[3]);

            return 0b00 << 30 | // TYPE
                0b10010 << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 << 10;  // OP2
        }

        case ROR: {
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = parse_reg(arg[3]);

            return 0b00 << 30 | // TYPE
                0b10011 << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 << 10;  // OP2
        }

        case ROL: {
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = parse_reg(arg[3]);

            return 0b00 << 30 | // TYPE
                0b10100 << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 << 10;  // OP2
        }
        case MUL: {
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = parse_reg(arg[3]);

            return 0b00 << 30 | // TYPE
                0b10101 << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 << 10;  // OP2
        }

        case UMUL: {
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = parse_reg(arg[3]);

            return 0b00 << 30 | // TYPE
                0b10110 << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 << 10;  // OP2
        }

        case DIV: {
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = parse_reg(arg[3]);

            return 0b00 << 30 | // TYPE
                0b10111 << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 << 10;  // OP2
        }

        case UDIV: {
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = parse_reg(arg[3]);

            return 0b00 << 30 | // TYPE
                0b11000 << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 << 10;  // OP2
        }

        case MOD: {
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = parse_reg(arg[3]);

            return 0b00 << 30 | // TYPE
                0b11001 << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 << 10;  // OP2
        }

        case UMOD: {
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t op1 = parse_reg(arg[2]);
            const uint32_t op2 = parse_reg(arg[3]);

            return 0b00 << 30 | // TYPE
                0b11010 << 25 | // OP
                    dst << 20 | // DST
                    op1 << 15 | // OP1
                    op2 << 10;  // OP2
        }

        case MOV: {
            const uint32_t op  = isReg(arg[2]) ? 0b11011 : 0b11100;
            const uint32_t dst = parse_reg(arg[1]);
            const uint32_t src = isReg(arg[2]) ?
                parse_reg(arg[2])     << 15 : // reg
                parse_imm(arg[2], 20) <<  0 ; // imm

            return 0b00 << 30 | // TYPE
                     op << 25 | // OP
                    dst << 20 | // DST
                    src <<  0;  // SRC
        }

        case TRN: {
            const uint32_t dst  = parse_reg(arg[1]);
            const uint32_t size = parse_size(arg[2]);

            return 0b00    << 30 | // TYPE
                   0b11101 << 25 | // OP
                    dst    << 20 | // DST
                   size    << 18;  // SIZE
        }

        // LOAD STORE INSTRUCTIONS

        case LDR: {
            const uint32_t size = parse_size(arg[1]);
            const uint32_t dst  = parse_reg(arg[2]);
            const uint32_t src  = parse_reg(arg[3]);

            return 0b01 << 30 | // TYPE
                      0 << 29 | // OP
                   size << 27 | // SIZE
                    dst << 22 | // DST
                    src << 17;  // SRC
        }
        case STR: {
            const uint32_t src  = parse_reg(arg[1]);
            const uint32_t dst  = parse_reg(arg[2]);

            return 0b01 << 30 | // TYPE
                      1 << 29 | // OP
                    src << 24 | // SRC
                    dst << 19;  // DST
        }

        // BRANCH INSTRUCTIONS

        case B: {
            const uint32_t cond = parse_cond(arg[1]);
            const uint32_t label = parse_label(arg[2], 20);

            return 0b10 << 30 | // TYPE
                   0b00 << 28 | // OP
                   cond << 20 | // COND
                   label << 0;  // LABEL
        }

        case BI: {
            const uint32_t cond = parse_cond(arg[1]);
            const uint32_t reg  = parse_reg(arg[2]);

            return 0b10 << 30 | // TYPE
                   0b01 << 28 | // OP
                   cond << 20 | // COND
                    reg << 15;  // REG
        }

        case BL: {
            const uint32_t label = parse_label(arg[1], 28);

            return 0b10 << 30 | // TYPE
                   0b10 << 28 | // OP
                   label << 0;  // LABEL
        }

        case CALL: {
            const uint32_t reg = parse_reg(arg[2]);

            return 0b10 << 30 | // TYPE
                   0b11 << 28 | // OP
                    reg << 23;  // REG
        }

        case HLT: {
            return 0b11 << 30; // TYPE
        }

        default: {
            cout << "Line " << line_num << ": invalid instruction '" << arg[0] << "'" << endl;
            exit(-1);
        }
    }
}

uint32_t Assembler::parse_reg(string& arg) {
    string reg = arg;

    // change reg to uppercase
    for (int i = 0; i < reg.size(); i++) {
        reg[i] = toupper(reg[i]);
    }

    if (reg == "R0")    return  0;
    if (reg == "R1")    return  1;
    if (reg == "R2")    return  2;
    if (reg == "R3")    return  3;
    if (reg == "R4")    return  4;
    if (reg == "R5")    return  5;
    if (reg == "R6")    return  6;
    if (reg == "R7")    return  7;
    if (reg == "R8")    return  8;
    if (reg == "R9")    return  9;
    if (reg == "R10")   return 10;
    if (reg == "R11")   return 11;
    if (reg == "R12")   return 12;
    if (reg == "R13")   return 13;
    if (reg == "R14")   return 14;
    if (reg == "R15")   return 15;
    if (reg == "R16")   return 16;
    if (reg == "R17")   return 17;
    if (reg == "R18")   return 18;
    if (reg == "R19")   return 19;
    if (reg == "R20")   return 20;
    if (reg == "R21")   return 21;
    if (reg == "R22")   return 22;
    if (reg == "R23")   return 23;
    if (reg == "R24")   return 24;
    if (reg == "R25")   return 25;
    if (reg == "R26")   return 26;
    if (reg == "R27")   return 27;
    if (reg == "R28")   return 28;
    if (reg == "R29")   return 29;
    if (reg == "R30")   return 30;
    if (reg == "R31")   return 31;

	if (reg == "LR")    return 28;
    if (reg == "SP")    return 29;
    if (reg == "FLAGS") return 30;
    if (reg == "PC")    return 31;

    cout << "Line " << line_num << ": invalid reg '" << arg << "'" <<endl;
    exit(-1);
}

uint32_t Assembler::parse_imm(string& arg, int num_bits) {
    assert(num_bits != 0);

    const int imm  = stoi(arg);
    const int mask = (1 << num_bits) - 1;

    return imm & mask;
}

uint32_t Assembler::parse_size(string& arg) {
    const int size = stoi(arg);

    if (size ==  8) return 0;
    if (size == 16) return 1;
    if (size == 32) return 2;

    cout << "Line " << line_num << ": invalid size '" << arg << "'" <<endl;
    exit(-1);
}

uint32_t Assembler::parse_cond(string& arg) {
    string cond = arg;

    // change reg to uppercase
    for (int i = 0; i < cond.size(); i++) {
        cond[i] = toupper(cond[i]);
    }

    if (cond == "AL") return 0;
    if (cond == "GT") return 1 << 0;
    if (cond == "LT") return 1 << 1;
    if (cond == "EQ") return 1 << 2;
    if (cond == "NE") return 1 << 3;
    if (cond == "OF") return 1 << 4;
    if (cond == "UF") return 1 << 5;
    if (cond == "CA") return 1 << 6;
    if (cond == "DZ") return 1 << 7;

    cout << "Line " << line_num << ": invalid cond '" << arg << "'" << endl;
    exit(-1);
}

int32_t Assembler::parse_label(string& arg, int num_bits) {
    assert(num_bits < 30);

    int32_t current_loc = program.size() + 1;
    int32_t label_loc   = labels[arg];

    const int mask = (1 << num_bits) - 1;

    return ((label_loc - current_loc) * sizeof(uint32_t)) & mask;
}
