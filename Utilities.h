#ifndef MIPS_UTILS_H
#define MIPS_UTILS_H
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

typedef unsigned char BYTE;
typedef unsigned short HALFWORD;
typedef unsigned int WORD;

namespace Parser {
    typedef std::vector<std::string> token_string;

    struct Line {
        std::unique_ptr<token_string> tokens; // Vector is a pointer into the heap so it can be passed in from the factory pattern
        // Constructor - just inherits token pointer
        Line(token_string& tokenized);

        virtual operator std::string() const; // For testing
    };

    struct InstructionLine : Line {
        BYTE opcode = 0, rs = 0, rt = 0, rd = 0, funct = 0;
        short imm = 0;

        InstructionLine(token_string& tokenized);
    };

    
    // Takes in a line and parses it into a specific "Line" object. Uses "line_num" for error-handling. Also a factory pattern
    std::unique_ptr<Line> create_line_from_parse(const std::string& line);

    // Parsing utils

    // Check for instruction existing (case matters!)
    bool valid_instruction(const std::string& instr);

    // Convert from register -> number
    BYTE reg_name_to_index(const std::string& reg);

    WORD instr_to_hex(const std::string& instr);

    std::string get_first_token(const std::string& line);

}

namespace Hardware {

    // Manages memory state
    struct RAM {
        std::vector<int> d_mem;                                         // Data Memory
        int rf[32] = {0};                                               // Register file, initialize all to 0
        WORD pc = 0;                                                    // Program Counter

        std::unordered_map<std::string, WORD> labels;                   // Mapping from labels to i_mem address
        std::vector<Parser::Line> i_mem;                                // Instruction Memory - Since is static throughout the whole program, we can store the non-compiled instruction
    };

}

namespace Utils {
    void error(const std::string& exit_msg);
    
    void assert(const bool& good_eval, const std::string& on_error);
}

#endif