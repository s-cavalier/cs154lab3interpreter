#include "Hardware.h"
#include <iostream>

Hardware::Instruction Hardware::instruction_builder(RAM& mem, const WORD& binary_instr) {
    BYTE opcode = static_cast<BYTE>(binary_instr >> 26);
    BYTE rs = static_cast<BYTE>((binary_instr >> 21) & 0b11111);
    BYTE rt = static_cast<BYTE>((binary_instr >> 16) & 0b11111);
    BYTE rd = static_cast<BYTE>((binary_instr >> 11) & 0b11111);
    BYTE funct = static_cast<BYTE>(binary_instr & 0b111111);
    short imm = static_cast<short>(binary_instr & 0xFFFF);

    if (opcode > 4) Utils::assert(rt > 0, "Attempted to write to $zero reg");
    if (opcode == 0 && funct != 0xC) Utils::assert(rd > 0, "Attempted to write to $zero reg");

    switch (opcode) {
        case 8: // ADDI
            return Instruction([rs, rt, imm] (RAM& mem) { mem.rf[rt] = mem.rf[rs] + static_cast<int>(imm); });
        case 0xf: // LUI
            return Instruction([rs, rt, imm] (RAM& mem) { mem.rf[rt] = static_cast<int>(imm) << 16; });
        case 0xd: // ORI
            return Instruction([rs, rt, imm] (RAM& mem) { mem.rf[rt] = mem.rf[rs] + (static_cast<int>(imm) & 0xFFFF); });
        case 0x23: // LW
            return Instruction([rs, rt, imm] (RAM& mem) { mem.rf[rt] = mem.d_mem[ mem.rf[rs] + static_cast<int>(imm) ]; });
        case 0x2b: // SW
            return Instruction([rs, rt, imm] (RAM& mem) { mem.d_mem[ mem.rf[rs] + static_cast<int>(imm) ] = mem.rf[rt];});
        case 0x4:
            return Instruction([rs, rt, imm] (RAM& mem) { mem.pc += static_cast<int>(imm); });
        default:
            break;
    }

    switch (funct) {
        case 0x20: // ADD
            return Instruction([rd, rs, rt] (RAM& mem) { mem.rf[rd] = mem.rf[rs] + mem.rf[rt]; });
        case 0x24: // AND
            return Instruction([rd, rs, rt] (RAM& mem) { mem.rf[rd] = mem.rf[rs] & mem.rf[rt]; });
        case 0x2a: // SLT
            return Instruction([rd, rs, rt] (RAM& mem) { mem.rf[rd] = (mem.rf[rs] < mem.rf[rt] ? 1 : 0); });
        case 0xC:
            return Instruction([] (RAM& mem) { mem.exit_flag = true; });
        default:
            break;
    }

    Utils::error("Somehow got bad opcode and funct for: " + std::to_string(binary_instr));

    return Instruction(0);

};