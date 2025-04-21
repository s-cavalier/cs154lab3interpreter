#include <vector>
#include <string>
#include <unordered_map>

typedef unsigned char BYTE;
typedef unsigned short HALFWORD;
typedef unsigned int WORD;

namespace Parser {
    typedef std::vector<std::string> token_string;

    // Check for instruction existing (case matters!)
    bool valid_instruction(const std::string& instr);

    // Convert from register -> number
    BYTE reg_name_to_index(const std::string& reg);

    WORD instr_to_hex(token_string& instr, const std::unordered_map<std::string, WORD>& label_table, const WORD& tmp_pc);

    WORD build_r_type(const BYTE& rd, const BYTE& rs, const BYTE& rt, const BYTE& funct);
    WORD build_i_type(const BYTE& opcode, const BYTE& rs, const BYTE& rt, const short& imm);

    std::string get_first_token(const std::string& line);

}

namespace Utils {
    void error(const std::string& exit_msg);
    
    void assert(const bool& good_eval, const std::string& on_error);
}