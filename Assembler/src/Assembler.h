#pragma once

#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class Assembler
{
    public:
        Assembler(const char* fname_in, const char* fname_out);
        ~Assembler();

        void parse();
        void write();
    private:
        bool isReg(string& arg) {
            return isalpha(arg[0]);
        }

        uint32_t parse_line(string& line);
        uint32_t parse_reg(string& arg);
        uint32_t parse_imm(string& arg, int num_bits);
        uint32_t parse_cond(string& arg);
        uint32_t parse_size(string& arg);
        int32_t  parse_label(string& arg, int num_bits);

        ofstream file_out;
        ifstream file_in;

        int line_num;

        vector<uint32_t> program;
        map<string, int32_t> labels;
};

enum TypeCode {
    ADD,
    SUB,
    RSUB,
    CMP,
    AND,
    OR,
    NOT,
    XOR,
    LSR,
    LSL,
    ASR,
    ROR,
    ROL,
    MUL,
    UMUL,
    DIV,
    UDIV,
    MOD,
    UMOD,
    MOV,
    TRN,
    LDR,
    STR,
    B,
    BI,
    BL,
    CALL,
    HLT
};

static map<string, TypeCode> INSTRUCTIONS = {
    {"ADD"  , ADD},
    {"SUB"  , SUB},
    {"RSUB" , RSUB},
    {"CMP"  , CMP},
    {"AND"  , AND},
    {"OR"   , OR},
    {"NOT"  , NOT},
    {"XOR"  , XOR},
    {"LSR"  , LSR},
    {"LSL"  , LSL},
    {"ASR"  , ASR},
    {"ROR"  , ROR},
    {"ROL"  , ROL},
    {"MUL"  , MUL},
    {"UMUL" , UMUL},
    {"DIV"  , DIV},
    {"UDIV" , UDIV},
    {"MOD"  , MOD},
    {"UMOD" , UMOD},
    {"MOV"  , MOV},
    {"TRN"  , TRN},
    {"LDR"  , LDR},
    {"STR"  , STR},
    {"B"    , B},
    {"BI"   , BI},
    {"BL"   , BL},
    {"CALL" , CALL},
    {"HLT"  , HLT},
    {"add"  , ADD},
    {"sub"  , SUB},
    {"rsub" , RSUB},
    {"cmp"  , CMP},
    {"and"  , AND},
    {"or"   , OR},
    {"not"  , NOT},
    {"xor"  , XOR},
    {"lsr"  , LSR},
    {"lsl"  , LSL},
    {"asr"  , ASR},
    {"ror"  , ROR},
    {"rol"  , ROL},
    {"mul"  , MUL},
    {"umul" , UMUL},
    {"div"  , DIV},
    {"udiv" , UDIV},
    {"mod"  , MOD},
    {"umod" , UMOD},
    {"mov"  , MOV},
    {"trn"  , TRN},
    {"ldr"  , LDR},
    {"str"  , STR},
    {"b"    , B},
    {"bi"   , BI},
    {"bl"   , BL},
    {"call" , CALL},
    {"hlt"  , HLT}
};

