#ifndef MIPS_HARDWARE_H
#define MIPS_HARDWARE_H
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include "Utils.h"

namespace Hardware {

    struct Instruction; // forward declaration to clear up circular dependency

    struct RAM {
        bool exit_flag = false;

        int rf[32] = {0};
        WORD pc = 0;
        
        std::unordered_map<WORD, int> d_mem;
        std::unordered_map<std::string, WORD> labels;
        std::vector<Instruction> i_mem;
    };

    struct Instruction {
        Instruction(std::function<void(RAM&)> callback) : compiled(callback) {};

        std::function<void(RAM&)> compiled;
    };

    Instruction instruction_builder(RAM& mem, const WORD& binary_instr);

}

#endif