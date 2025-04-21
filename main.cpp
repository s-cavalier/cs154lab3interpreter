#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Utils.h"
#include "Hardware.h"

using namespace std;
using namespace Utils; // Only use namespace utils, since these should be global here but exist in namespace in Utils.h for cleaner codespace

int main(int argc, char** argv) {

    // Check for valid input
    assert(argc > 1, "USAGE: ./interpret <MIPS src> <output file>");

    // First is to compile into both internal memory and pyrtl CPU mem

    ifstream MIPS_src(argv[1]);
    assert(MIPS_src.good(), "Failed to open file " + string(argv[1]) + ".");

    ofstream output_file(argv[2]);
    assert(output_file.good(), "Failed to open file " + string(argv[2]) + ".");

    // First pass, deign labels, PC, and basic error-checks
    unordered_map<string, WORD> label_table;
    vector<string> seen_labels;
    WORD tmp_pc = 0;
    WORD line_counter = 0;
    string line;
    while(getline(MIPS_src, line)) {
        ++line_counter;
        string first_token = Parser::get_first_token(line);
        if (first_token.empty()) continue;
        if (first_token[0] == '#') continue;

        bool is_instr = Parser::valid_instruction(first_token);
        bool is_directive = (first_token == ".text");
        bool is_label = (first_token.back() == ':');
        assert(is_instr || is_directive || is_label, "Couldn't identify syntax at line " + to_string(line_counter) + ". Maybe disallowed instruction:\n" + line);
        
        if (is_directive) continue;

        if (is_label) {
            first_token.pop_back();
            seen_labels.push_back(first_token);
            continue;
        }
        
        while (!seen_labels.empty()) {
            label_table[seen_labels.back()] = tmp_pc;
            seen_labels.pop_back();
        }

        ++tmp_pc;
    }

    Hardware::RAM mem;


    assert(seen_labels.size() == 1, "Reading labels with no instructions after them. Check:\n" + seen_labels.front() + ':');
    assert(seen_labels[0] == "exit", "No 'exit:' label found. Please make an 'exit:' label and jump to the label with beq to exit the program.\nWhen the label is called, the program will end, so no code after will be executed. It is effectively the same as exiting with syscall.");

    // Artifically add an extra instruction at the end of the program so the Python CPU closes
    label_table["exit"] = tmp_pc;

    // Second pass, Compile instructions
    MIPS_src.close();
    MIPS_src.open(argv[1]);
    output_file << std::hex;
    tmp_pc = 0;

    while(getline(MIPS_src, line)) {
        if (line.empty()) continue;

        istringstream splitter(line);
        Parser::token_string tokens;
        string token;

        while(splitter >> token) {
            if (token[0] == '#') break;
            tokens.push_back(token);
        }

        if (tokens.empty()) continue;
        if (!Parser::valid_instruction(tokens[0])) continue;

        WORD instr = Parser::instr_to_hex(tokens, label_table, tmp_pc);

        output_file << setw(8) << setfill('0') << instr << '\n';
        ++tmp_pc;
    }

    Parser::token_string extra_instruction = {"beq", "$zero,", "$zero,", "exit"};
    output_file << setw(8) << setfill('0') << Parser::instr_to_hex(extra_instruction, label_table, tmp_pc);
    

    return 0;
}