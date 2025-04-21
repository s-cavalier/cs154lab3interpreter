#include "Hardware.h"

Hardware::Instruction Hardware::instruction_builder(RAM& mem, const WORD& binary_instr) {
    BYTE opcode = static_cast<BYTE>(binary_instr >> 26);
    BYTE rs = binary_instr & 0x03E00000;
    BYTE rt = binary_instr & 0x001F0000;
    BYTE rd = binary_instr & 0x0000F800;
    BYTE funct = binary_instr & 0x0000003F;
    short imm = static_cast<short>(binary_instr & 0xFFFF);

    switch (opcode) {
        case 8: // ADDI
            return Instruction([rs, rt, imm] (RAM& mem) { mem.rf[rt] = mem.rf[rs] + static_cast<int>(imm); });
        case 0xf: // LUI
            return Instruction([rs, rt, imm] (RAM& mem) { mem.rf[rt] = mem.rf[rs] + static_cast<int>(imm); }); // TODO: LUI needs fixing everyone since it has special syntax
        case 0xd: // ORI
            return Instruction([rs, rt, imm] (RAM& mem) { mem.rf[rt] = mem.rf[rs] + (static_cast<int>(imm) & 0xFFFF); });
        case 0x23: // LW
            return Instruction([rs, rt, imm] (RAM& mem) { mem.rf[rt] = mem.d_mem[ mem.rf[rs] + static_cast<int>(imm) ]; });
        case 0x2b: // SW
            return Instruction([rs, rt, imm] (RAM& mem) { mem.d_mem[ mem.rf[rs] + static_cast<int>(imm) ] = mem.rf[rt]; });
        case 0x4:
            return Instruction([rs, rt, imm] (RAM& mem) { mem.pc += 1 + static_cast<int>(imm); });
        default:
            break;
    }

    switch (funct) {
        case 0x20:
            return Instruction([rd, rs, rt] (RAM& mem) { mem.rf[rd] = mem.rf[rs] + mem.rf[rt]; });
        case 0x24:
            return Instruction([rd, rs, rt] (RAM& mem) { mem.rf[rd] = mem.rf[rs] & mem.rf[rt]; });
        case 0x2a:
            return Instruction([rd, rs, rt] (RAM& mem) { mem.rf[rd] = (mem.rf[rs] < mem.rf[rt] ? 1 : 0); });
        default:
            break;
    }

    Utils::error("Somehow got bad opcode and funct for: " + binary_instr);

    return Instruction(0);

};