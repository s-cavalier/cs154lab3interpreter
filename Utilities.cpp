#include "Utilities.h"
#include <sstream>
#include <iostream>
using namespace std;

// PARSER

Parser::Line::Line(token_string& tokenized) {
    tokens = unique_ptr<Parser::token_string>(&tokenized);
}

Parser::Line::operator string() const {
    string out;
    for (const auto& str : *tokens) {
        out += str;
        out.push_back(' ');
    }
    return out;
}

Parser::InstructionLine::InstructionLine(token_string& tokenized) : Line(tokenized) {

    // get opcode
    if (tokens->at(0) == "addi") opcode = 0x8;
    if (tokens->at(0) == "lui") opcode = 0xf;
    if (tokens->at(0) == "ori") opcode = 0xd;
    if (tokens->at(0) == "lw") opcode = 0x23;
    if (tokens->at(0) == "sw") opcode = 0x2b;
    if (tokens->at(0) == "beq") opcode = 0x4;

    // get funct
    if (tokens->at(0) == "add") funct = 0x20;
    if (tokens->at(0) == "and") funct = 0x24;
    if (tokens->at(0) == "slt") funct = 0x2a;

    // TODO: Refactor for factory pattern / better polymorphism (for interpreter)

    bool R_TYPE = (opcode == 0);

    if (R_TYPE) {
        Utils::assert(tokens->at(1).back() == ',', "Missing comma:\n" + string(*this));
        tokens->at(1).pop_back();
        Utils::assert(tokens->at(2).back() == ',', "Missing comma:\n" + string(*this));
        tokens->at(2).pop_back();

        rd = reg_name_to_index(tokens->at(1));
        rs = reg_name_to_index(tokens->at(2));
        rt = reg_name_to_index(tokens->at(3));
    } else if (tokens->at(0) == "lw" || tokens->at(0) == "sw") {
        Utils::assert(tokens->at(1).back() == ',', "Missing comma:\n" + string(*this));
        tokens->at(1).pop_back();

        rt = reg_name_to_index(tokens->at(1));
        const string& imm_reg_reference = tokens->at(2);
        WORD end_of_imm = imm_reg_reference.find_first_of('(');
        WORD end_of_string = imm_reg_reference.find_first_of(')', end_of_imm);

        int immediate_val = stoi( imm_reg_reference.substr(0, end_of_imm) );
        Utils::assert(immediate_val <= 32767 && immediate_val >= -32768, "immediate value out of range (-32768, 32767):\n" + string(*this));
        imm = static_cast<short>(immediate_val);

        rs = reg_name_to_index( imm_reg_reference.substr(end_of_imm, end_of_string - end_of_imm) );
    } else {
        Utils::assert(tokens->at(1).back() == ',', "Missing comma:\n" + string(*this));
        tokens->at(1).pop_back();
        Utils::assert(tokens->at(2).back() == ',', "Missing comma:\n" + string(*this));
        tokens->at(2).pop_back();

        rt = reg_name_to_index(tokens->at(1));
        rs = reg_name_to_index(tokens->at(2));

        int immediate_val = stoi( tokens->at(3) );
        Utils::assert(immediate_val <= 32767 && immediate_val >= -32768, "immediate value out of range (-32768, 32767):\n" + string(*this));
        imm = static_cast<short>(immediate_val);
    }

}

unique_ptr<Parser::Line> Parser::create_line_from_parse(const string& line) {
    auto tokenized_line = new Parser::token_string;

    string token;
    istringstream token_parser(line);
    while(token_parser >> token) {
        if (token[0] == '#') break;
        if (token[0] == '.') Utils::assert(token == ".text", "Only directive allowed is .text at:\n" + line);
        tokenized_line->push_back(token);
    }

    // Parsed - now create object

    if (tokenized_line->empty()) { delete tokenized_line; return nullptr; }
    if (Parser::valid_instruction(tokenized_line->front())) return make_unique<Parser::InstructionLine>(*tokenized_line);

    return make_unique<Parser::Line>(*tokenized_line);
}

bool Parser::valid_instruction(const string& instr) {
    // giant return statement - since its only like 9 instructions idrc it is readable, fast, and inlineable (?) and that is what matters
    // plus the alternatives are for loop or using a hash set, both less readable
    return
    (instr == "add") ||
    (instr == "and") ||
    (instr == "addi") ||
    (instr == "lui") ||
    (instr == "ori") ||
    (instr == "slt") ||
    (instr == "lw") ||
    (instr == "sw") ||
    (instr == "beq");
}

BYTE Parser::reg_name_to_index(const std::string& reg) {
    // Basically just a switch case, but not a thing in C++ so this is how it is
    if (reg == "$zero") return 0;
    if (reg == "$at") return 1;
    if (reg == "$v0") return 2;
    if (reg == "$v1") return 3;
    if (reg == "$a0") return 4;
    if (reg == "$a1") return 5;
    if (reg == "$a2") return 6;
    if (reg == "$a3") return 7;
    if (reg == "$t0") return 8;
    if (reg == "$t1") return 9;
    if (reg == "$t2") return 10;
    if (reg == "$t3") return 11;
    if (reg == "$t4") return 12;
    if (reg == "$t5") return 13;
    if (reg == "$t6") return 14;
    if (reg == "$t7") return 15;
    if (reg == "$s0") return 16;
    if (reg == "$s1") return 17;
    if (reg == "$s2") return 18;
    if (reg == "$s3") return 19;
    if (reg == "$s4") return 20;
    if (reg == "$s5") return 21;
    if (reg == "$s6") return 22;
    if (reg == "$s7") return 23;
    if (reg == "$t8") return 24;
    if (reg == "$t9") return 25;
    if (reg == "$k0") return 26;
    if (reg == "$k1") return 27;
    if (reg == "$gp") return 28;
    if (reg == "$sp") return 29;
    if (reg == "$fp") return 30;
    if (reg == "$ra") return 31;

    Utils::error("Bad register: " + reg);

    return -1;
}

WORD Parser::instr_to_hex(const std::string& instr) {
    return 0;
}

std::string Parser::get_first_token(const std::string& line) {
    if (line.empty()) return "";
    WORD start = line.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    WORD end = line.find_first_of(" \t\n\r", start);
    
    return line.substr(start, end - start);
}

// UTILS

void Utils::assert(const bool& good_eval, const string& on_error) {
    if (good_eval) return;
    Utils::error(on_error);
}

void Utils::error(const std::string& exit_msg) {
    cerr << "Encountered error:\n" << exit_msg << endl;
    exit(1);
}