#include <stdio.h>
#include "pshell.h"
#include "terminal.h"

#ifdef __PC
#define t_printf       printf
#else
#define t_printf       tprintf
#endif

#define I_OPCODE(op)    ((op >> 26) & 0x3F)
#define I_PARAM_0(op)   ((op >> 21) & 0x1F)
#define I_PARAM_1(op)   ((op >> 16) & 0x1F)
#define I_PARAM_2(op)   ((op >> 11) & 0x1F)
#define I_PARAM_3(op)   ((op >> 6) & 0x1F)
#define I_PARAM_4(op)   (op & 0x3F)

#define I_IMM16(op)      (op & 0xFFFF)
#define I_IMM20(op)      ((op >> 6) & 0xFFFFF)
#define IMM_26(op)       (op & 0x03FFFFFF)

const char genRegNames[32][5] = {
    "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3",
    "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8",
    "t9", "k0", "k1", "gp", "sp", "fp", "ra" };

const char COP0_RegNames[32][16] = {
    "Index", "Random", "EntryLo0", "EntryLo1", "Context", "PageMask", "Wired", "COP0_r7", "BadVAddr",
    "Count", "EntryHi", "Compare", "Status", "Cause", "EPC", "PRId", "Config",
    "COP0_r16", "COP0_r17", "COP0_r18", "COP0_r19", "COP0_r20", "COP0_r21", "COP0_r22",
    "BadPAddr", "Debug", "PCCR", "PCR0", "PCR1", "TagLo", "TagHi", "ErrorEPC" };

const char Debug_RegNames[8][5] = {
    "bpc", "inv", "iab", "iabm", "dab", "dabm", "dvb", "dvbm" };

enum {
    OP_SPECIAL      = 0,
    OP_REGIMM       = 1,
    OP_J            = 2,
    OP_JAL          = 3,
    OP_BEQ          = 4,
    OP_BNE          = 5,
    OP_BLEZ         = 6,
    OP_BGTZ         = 7,

    OP_ADDI         = 8,
    OP_ADDIU        = 9,
    OP_SLTI         = 10,
    OP_SLTUI        = 11,
    OP_ANDI         = 12,
    OP_ORI          = 13,
    OP_XORI         = 14,
    OP_LUI          = 15,

    OP_COP0         = 16,
    OP_COP1         = 17,
    OP_COP2         = 18,
    OP_INV19        = 19,
    OP_BEQL         = 20,
    OP_BNEL         = 21,
    OP_BLEZI        = 22,
    OP_BGTZL        = 23,

    OP_DADDI        = 24,
    OP_DADDIU       = 25,
    OP_LDL          = 26,
    OP_LDR          = 27,
    OP_MMI          = 28,
    OP_INV29        = 29,
    OP_LQ           = 30,
    OP_SQ           = 31,

    OP_LB           = 32,
    OP_LH           = 33,
    OP_LWL          = 34,
    OP_LW           = 35,
    OP_LBU          = 36,
    OP_LHU          = 37,
    OP_LWR          = 38,
    OP_LWU          = 39,

    OP_SB           = 40,
    OP_SH           = 41,
    OP_SWL          = 42,
    OP_SW           = 43,
    OP_SDL          = 44,
    OP_SDR          = 45,
    OP_SWR          = 46,
    OP_CACHE        = 47,

    OP_INV48        = 48,
    OP_LWC1         = 49,
    OP_INV50        = 50,
    OP_PREF         = 51,
    OP_ALPHA10      = 52,
    OP_BETA5        = 53,
    OP_BETA6        = 54,
    OP_ALPHA11      = 55,

    OP_SC           = 56,
    OP_BETA7        = 57,
    OP_BETA8        = 58,
    OP_ALPHA12      = 59,
    OP_ALPHA13      = 60,
    OP_BETA9        = 61,
    OP_BETA10       = 62,
    OP_ALPHA14      = 63,
};

enum {
    SPEC_SLL    = 0,
    SPEC_BETA0    = 1,
    SPEC_SRL    = 2,
    SPEC_SRA    = 3,
    SPEC_SLLV    = 4,
    SPEC_ALPHA0    = 5,
    SPEC_SRLV    = 6,
    SPEC_SRAV    = 7,

    SPEC_JR        = 8,
    SPEC_JALR    = 9,
    SPEC_MOVZ    = 10,
    SPEC_MOVN    = 11,
    SPEC_SYSCALL= 12,
    SPEC_BREAK    = 13,
    SPEC_ALPHA1    = 14,
    SPEC_SYNC    = 15,

    SPEC_MFHI    = 16,
    SPEC_MTHI    = 17,
    SPEC_MFLO    = 18,
    SPEC_MTLO    = 19,
    SPEC_ALPHA2    = 20,
    SPEC_ALPHA3    = 21,
    SPEC_ALPHA4    = 22,
    SPEC_ALPHA5    = 23,

    SPEC_MULT    = 24,
    SPEC_MULTU    = 25,
    SPEC_DIV    = 26,
    SPEC_DIVU    = 27,
    SPEC_ALPHA6    = 28,
    SPEC_ALPHA7    = 29,
    SPEC_ALPHA8    = 30,
    SPEC_ALPHA9    = 31,

    SPEC_ADD    = 32,
    SPEC_ADDU    = 33,
    SPEC_SUB    = 34,
    SPEC_SUBU    = 35,
    SPEC_AND    = 36,
    SPEC_OR        = 37,
    SPEC_XOR    = 38,
    SPEC_NOR    = 39,

    SPEC_ALPHA10= 40,
    SPEC_ALPHA11= 41,
    SPEC_SLT    = 42,
    SPEC_SLTU    = 43,
    SPEC_ALPHA12= 44,
    SPEC_ALPHA13= 45,
    SPEC_ALPHA14= 46,
    SPEC_ALPHA15= 47,

    SPEC_TGE    = 48,
    SPEC_TGEU    = 49,
    SPEC_TLT    = 50,
    SPEC_TLTU    = 51,
    SPEC_TEQ    = 52,
    SPEC_ALPHA16= 53,
    SPEC_TNE    = 54,
    SPEC_ALPHA17= 55,

    SPEC_ALPHA18= 56,
    SPEC_ALPHA19= 57,
    SPEC_ALPHA20= 58,
    SPEC_ALPHA21= 59,
    SPEC_ALPHA22= 60,
    SPEC_ALPHA23= 61,
    SPEC_ALPHA24= 62,
    SPEC_ALPHA25= 63,
};

enum {
    SPEC2_MADD        = 0,
    SPEC2_MADDU        = 1,
    SPEC2_MUL        = 2,
    SPEC2_ALPHA0    = 3,
    SPEC2_MSUB        = 4,
    SPEC2_MSUBU        = 5,
    SPEC2_ALPHA1    = 6,
    SPEC2_ALPHA2    = 7,

    SPEC2_ALPHA3    = 8,
    SPEC2_ALPHA4    = 9,
    SPEC2_ALPHA5    = 10,
    SPEC2_ALPHA6    = 11,
    SPEC2_ALPHA7    = 12,
    SPEC2_ALPHA8    = 13,
    SPEC2_ALPHA9    = 14,
    SPEC2_ALPHA10    = 15,

    SPEC2_ALPHA11    = 16,
    SPEC2_ALPHA12    = 17,
    SPEC2_ALPHA13    = 18,
    SPEC2_ALPHA14    = 19,
    SPEC2_ALPHA15    = 20,
    SPEC2_ALPHA16    = 21,
    SPEC2_ALPHA17    = 22,
    SPEC2_ALPHA18    = 23,

    SPEC2_ALPHA19    = 24,
    SPEC2_ALPHA20    = 25,
    SPEC2_ALPHA21    = 26,
    SPEC2_ALPHA22    = 27,
    SPEC2_ALPHA23    = 28,
    SPEC2_ALPHA24    = 29,
    SPEC2_ALPHA25    = 30,
    SPEC2_ALPHA26    = 31,

    SPEC2_CLZ        = 32,
    SPEC2_CLO        = 33,

    SPEC2_SDBBP        = 63,
};

enum {
    REGIMM_BLTZ        = 0,
    REGIMM_BGEZ        = 1,
    REGIMM_BLTZL    = 2,
    REGIMM_BGEZL    = 3,

    REGIMM_TGEI        = 8,
    REGIMM_TGEIU    = 9,
    REGIMM_TLTI        = 10,
    REGIMM_TLTIU    = 11,
    REGIMM_TEQI        = 12,

    REGIMM_TNEI        = 14,

    REGIMM_BLTZAL    = 16,
    REGIMM_BGEZAL    = 17,
    REGIMM_BLTZALL    = 18,
    REGIMM_BGEZALL    = 19,
};

void decode_special(u32 op, u32 pc, u32 flags);
void decode_mmi(u32 op, u32 pc, u32 flags);
void decode_regimm(u32 op, u32 pc, u32 flags);
void decode_imm26(u32 op, u32 pc, u32 flags);
void decode_imm16(u32 op, u32 pc, u32 flags);
void decode_1_reg_imm16(u32 op, u32 pc, u32 flags);
void decode_cop0(u32 op, u32 pc, u32 flags);
void decode_cacheop(u32 op, u32 pc, u32 flags);
void decode_pref(u32 op, u32 pc, u32 flags);
void decode_syscall(u32 op, u32 pc, u32 flags);
void decode_break(u32 op, u32 pc, u32 flags);
void invalid_instruction(u32 op, u32 pc, u32 flags);

void decode_sync(u32 op, u32 pc, u32 flags);

void decode_1_reg(u32 op, u32 pc, u32 flags);
void decode_2_reg(u32 op, u32 pc, u32 flags);
void decode_2_reg_imm5(u32 op, u32 pc, u32 flags);
void decode_3_reg(u32 op, u32 pc, u32 flags);

#define OPCODE_TYPE_JUMP            0
#define OPCODE_TYPE_ALU             1
#define OPCODE_TYPE_COP0            2
#define OPCODE_TYPE_CACHE           3
#define OPCODE_TYPE_PREFETCH        4
#define OPCODE_TYPE_LOADSTORE       5
#define OPCODE_TYPE_BRANCH1         6
#define OPCODE_TYPE_BRANCH2         7
#define OPCODE_TYPE_REGIMM16        8
#define OPCODE_TYPE_MF0             9
#define OPCODE_TYPE_MT0             10

#define OPCODE_TYPE_INVALID         0xFF

#define OPCODE_TYPE_IMM26           (0 << 8)
#define OPCODE_TYPE_IMM16           (1 << 8)
#define OPCODE_TYPE_REG             (2 << 8)


struct main_decode_table_entry {
    const char *mnem;
    void (*chain)(u32 op, u32 pc, u32 flags);
    u32 flags;
} main_decode_table[64] = {
    {NULL, decode_special, 0},
    {NULL, decode_regimm, 0},

    {"j",       decode_imm26, OPCODE_TYPE_IMM26 | OPCODE_TYPE_JUMP},
    {"jal",     decode_imm26, OPCODE_TYPE_IMM26 | OPCODE_TYPE_JUMP},
    {"beq",     decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_BRANCH1},
    {"bne",     decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_BRANCH1},
    {"blez",    decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_BRANCH2},
    {"bgtz",    decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_BRANCH2},

    {"addi",    decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_ALU},
    {"addiu",   decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_ALU},
    {"slti",    decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_ALU},
    {"sltiu",   decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_ALU},
    {"andi",    decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_ALU},
    {"ori",     decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_ALU},
    {"xori",    decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_ALU},
    {"lui",     decode_1_reg_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_REGIMM16},

    {NULL,      decode_cop0, OPCODE_TYPE_COP0},
    {"cop1",    decode_cop0, OPCODE_TYPE_COP0},
    {"cop2",    decode_cop0, OPCODE_TYPE_COP0},
    {NULL,      invalid_instruction, OPCODE_TYPE_INVALID},
    {"beql",    decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_BRANCH1},
    {"bnel",    decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_BRANCH1},
    {"blezl",   decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_BRANCH2},
    {"bgtzl",   decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_BRANCH2},

    {"daddi",   decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_ALU},
    {"daddiu",  decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_ALU},
    {"ldl",     decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_LOADSTORE},
    {"ldr",     decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_LOADSTORE},
    {NULL,      decode_mmi, 0}, // MMI
    {NULL,      invalid_instruction, OPCODE_TYPE_INVALID},
    {"lq",      decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_LOADSTORE},
    {"sq",      decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_LOADSTORE},

    {"lb",      decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_LOADSTORE},
    {"lh",      decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_LOADSTORE},
    {"lwl",     decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_LOADSTORE},
    {"lw",      decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_LOADSTORE},
    {"lbu",     decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_LOADSTORE},
    {"lhu",     decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_LOADSTORE},
    {"lwr",     decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_LOADSTORE},
    {"lwu",     decode_imm16, OPCODE_TYPE_IMM16 | OPCODE_TYPE_LOADSTORE},

    {"sb",      decode_imm16, OPCODE_TYPE_IMM16|OPCODE_TYPE_LOADSTORE},
    {"sh",      decode_imm16, OPCODE_TYPE_IMM16|OPCODE_TYPE_LOADSTORE},
    {"swl",     decode_imm16, OPCODE_TYPE_IMM16|OPCODE_TYPE_LOADSTORE},
    {"sw",      decode_imm16, OPCODE_TYPE_IMM16|OPCODE_TYPE_LOADSTORE},

    {NULL,      invalid_instruction, OPCODE_TYPE_INVALID},
    {NULL,      invalid_instruction, OPCODE_TYPE_INVALID},
    {"swr",     decode_imm16, OPCODE_TYPE_IMM16|OPCODE_TYPE_LOADSTORE},
    {"cache",   decode_cacheop, OPCODE_TYPE_IMM16|OPCODE_TYPE_CACHE},

    {NULL,      invalid_instruction, OPCODE_TYPE_INVALID},
    {"lwc1",    decode_imm16, OPCODE_TYPE_IMM16|OPCODE_TYPE_LOADSTORE},
    {NULL,      invalid_instruction, OPCODE_TYPE_INVALID},
    {"pref",    decode_pref, OPCODE_TYPE_IMM16|OPCODE_TYPE_PREFETCH},

    {NULL,      invalid_instruction, OPCODE_TYPE_INVALID},
    {NULL,      invalid_instruction, OPCODE_TYPE_INVALID},
    {"lqc2",    decode_imm16, OPCODE_TYPE_IMM16|OPCODE_TYPE_LOADSTORE},
    {"ld",      decode_imm16, OPCODE_TYPE_IMM16|OPCODE_TYPE_LOADSTORE},

    {NULL,      invalid_instruction, OPCODE_TYPE_INVALID},
    {"swc1",    decode_imm16, OPCODE_TYPE_IMM16|OPCODE_TYPE_LOADSTORE},
    {NULL,      invalid_instruction, OPCODE_TYPE_INVALID},
    {NULL,      invalid_instruction, OPCODE_TYPE_INVALID},

    {"sc",      decode_imm16, OPCODE_TYPE_IMM16|OPCODE_TYPE_LOADSTORE},
    {NULL,      invalid_instruction, OPCODE_TYPE_INVALID},
    {"sqc2",    decode_imm16, OPCODE_TYPE_IMM16|OPCODE_TYPE_LOADSTORE},
    {"sd",      decode_imm16, OPCODE_TYPE_IMM16|OPCODE_TYPE_LOADSTORE},
};

struct special_decode_table_entry {
    const char *mnem;
    void (*chain)(u32 op, u32 pc, u32 flags);
    u32 flags;
} special_decode_table[64] = {
    {"sll", decode_2_reg_imm5, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {NULL, invalid_instruction, 0},
    {"srl", decode_2_reg_imm5, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"sra", decode_2_reg_imm5, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"sllv", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {NULL, invalid_instruction, 0},
    {"srlv", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"srav", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},

    {"jr", decode_1_reg, OPCODE_TYPE_REG|OPCODE_TYPE_JUMP},
    {"jalr", decode_1_reg, OPCODE_TYPE_REG|OPCODE_TYPE_JUMP},
    {"movz", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"movn", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"syscall", decode_syscall, 0},
    {"break", decode_break, 0},
    {NULL, invalid_instruction, 0},
    {NULL, decode_sync, 0},

    {"mfhi", decode_1_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"mthi", decode_1_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"mflo", decode_1_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"mthi", decode_1_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {NULL, invalid_instruction, 0},
    {NULL, invalid_instruction, 0},
    {NULL, invalid_instruction, 0},
    {NULL, invalid_instruction, 0},

    {"mult", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"multu", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"div", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"divu", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {NULL, invalid_instruction, 0},
    {NULL, invalid_instruction, 0},
    {NULL, invalid_instruction, 0},
    {NULL, invalid_instruction, 0},

    {"add", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"addu", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"sub", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"subu", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"and", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"or", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"xor", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"nor", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},

    {"mfsa", decode_1_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"mtsa", decode_1_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"slt", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"sltu", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"dadd", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"daddu", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"dsub", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"dsubu", decode_3_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},

    {"tge", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"tgeu", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"tlt", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"tltu", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"teq", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {NULL, invalid_instruction, 0},
    {"tne", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {NULL, invalid_instruction, 0},

    {"dsll", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {NULL, invalid_instruction, 0},
    {"dsrl", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"dsra", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"dsll32", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {NULL, invalid_instruction, 0},
    {"dsrl32", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
    {"dsra32", decode_2_reg, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},
};

void decode_mx0(u32 op, u32 pc, u32 flags);
void decode_bc0(u32 op, u32 pc, u32 flags);
void decode_c0(u32 op, u32 pc, u32 flags);

struct cop0_decode_table_entry {
    const char *mnem;
    void (*chain)(u32 op, u32 pc, u32 flags);
    u32 flags;
} cop0_decode_table[32] = {
    { NULL, decode_mx0, OPCODE_TYPE_MF0 }, // MF0
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},

    { NULL, decode_mx0, OPCODE_TYPE_MT0 }, // MT0
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},

    { NULL, decode_bc0, OPCODE_TYPE_REG | OPCODE_TYPE_ALU}, // BC0
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},

    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},

    { NULL, decode_c0, OPCODE_TYPE_REG|OPCODE_TYPE_ALU},  // C0
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},

    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},

    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
    { NULL, invalid_instruction, 0},
};

struct bc0_decode_table_entry {
    const char *mnem;
    void (*chain)(u32 op, u32 pc, u32 flags);
    u32 flags;
} bc0_decode_table[4] = {
    { "bc0f", decode_imm16, 0},
    { "bc0t", decode_imm16, 0},
    { "bc0fl", decode_imm16, 0},
    { "bc0ft", decode_imm16, 0},
};


struct regimm_decode_table_entry {
    const char *mnem;
    void (*chain)(u32 op, u32 pc, u32 flags);
    u32 flags;
} regimm_decode_table[64] = {
    {"bltz", decode_imm16, OPCODE_TYPE_REG | OPCODE_TYPE_BRANCH2 },
    {"bgez", decode_imm16, OPCODE_TYPE_REG | OPCODE_TYPE_BRANCH2 },
    {"bltzl", decode_imm16, OPCODE_TYPE_REG | OPCODE_TYPE_BRANCH2 },
    {"bgezl", decode_imm16, OPCODE_TYPE_REG | OPCODE_TYPE_BRANCH2 },

    {NULL, invalid_instruction, 0},
    {NULL, invalid_instruction, 0},
    {NULL, invalid_instruction, 0},
    {NULL, invalid_instruction, 0},

    {"bltzl", decode_imm16, OPCODE_TYPE_REG },

};


void decode_mx0(u32 op, u32 pc, u32 flags) {

    unsigned int rd = I_PARAM_2(op);

    if(rd == 24) { // Debug instruction
        rd = (op & 0x3FF);

        if((flags & 0xFF) == OPCODE_TYPE_MF0)
            t_printf("mf%-11s", Debug_RegNames[rd]);
        else
            t_printf("mt%-11s", Debug_RegNames[rd]);

        t_printf("%s", genRegNames[I_PARAM_1(op)]);
        }
    else {
        if((flags & 0xFF) == OPCODE_TYPE_MT0)
            t_printf("mtc0");
        else
            t_printf("mfc0");

        t_printf("         %s, %s", genRegNames[I_PARAM_1(op)], COP0_RegNames[rd]);
        }
}

void decode_bc0(u32 op, u32 pc, u32 flags) {
    struct bc0_decode_table_entry *ent;

    if(I_PARAM_1(op) & 0x18)
        t_printf("invalid");
    else {
        ent = &bc0_decode_table[I_PARAM_1(op)];

        if(ent->mnem)
            t_printf("%-12s", ent->mnem);

        if(ent->chain)
            ent->chain(op, pc, ent->flags);
        }
}

void decode_c0(u32 op, u32 pc, u32 flags) {

    switch(I_PARAM_4(op)) {
        case 0x01:
            t_printf("tlbr");
            break;
        case 0x02:
            t_printf("tlbwi");
            break;
        case 0x06:
            t_printf("tlbwr");
            break;
        case 0x08:
            t_printf("tlbp");
            break;
        case 0x18:
            t_printf("eret");
            break;
        case 0x38:
            t_printf("ei");
            break;
        case 0x39:
            t_printf("di");
            break;
        default:
            t_printf(".word        0x%08X", op);
            break;
        }
}

void decode_opcode(u32 op, u32 pc) {
    struct main_decode_table_entry *ent;
    u32 topbits = (op >> 26);

    if(0 == op) {
        t_printf("nop\n");
        }
    else {
        ent = &main_decode_table[topbits];

        if(ent->mnem)
            t_printf("%-12s", ent->mnem);

        if(ent->chain)
            ent->chain(op, pc, ent->flags);

        t_printf("\n");
        }
}

void decode_special(u32 op, u32 pc, u32 flags) {
    struct special_decode_table_entry *ent;
    u32 lowbits = op&0x3f;
    ent = &special_decode_table[lowbits];

    if(ent->mnem)
        t_printf("%-12s", ent->mnem);

    if(ent->chain)
        ent->chain(op, pc, ent->flags);
}

void decode_regimm(u32 op, u32 pc, u32 flags) {
    struct regimm_decode_table_entry *ent;

    u8 code = I_PARAM_1(op);

    ent = &regimm_decode_table[code];

    if(ent->mnem)
        t_printf("%-12s", ent->mnem);

    if(ent->chain)
        ent->chain(op, pc, ent->flags);
}

void decode_mmi(u32 op, u32 pc, u32 flags) {
    t_printf("mmi");
}

void decode_1_reg_imm16(u32 op, u32 pc, u32 flags) {
    t_printf(" %s, 0x%04X", genRegNames[I_PARAM_1(op)], I_IMM16(op));
}

void decode_imm26(u32 op, u32 pc, u32 flags) {
    u32 imm26 = (op & ((1 << 26) - 1)) << 2;
    imm26 |= 0x80000000;
    t_printf(" 0x%08X", imm26);
}

void decode_imm16(u32 op, u32 pc, u32 flags) {
    u32 imm16;

    imm16 = op & 0x0000FFFF;

    if((flags & 0xFF) == OPCODE_TYPE_LOADSTORE) {
        u32 rt, base;
        rt = (op >> 16) & 0x1F;
        base = (op >> 21) & 0x1F;
        t_printf(" %s, 0x%04X(%s)", genRegNames[rt], imm16, genRegNames[base]);
        }
    else {
        if(OPCODE_TYPE_BRANCH1 == (flags & 0xFF)) {
            s16 offs = imm16;

            decode_2_reg(op, pc, flags);

            t_printf(", 0x%08X", pc + 4 + (offs * 4));
            }
        else if(OPCODE_TYPE_BRANCH2 == (flags & 0xFF)) {
            s16 offs = imm16;

            t_printf(" %s, 0x%08X", genRegNames[I_PARAM_0(op)], pc + 4 + (offs * 4));
            }
        else {
            decode_2_reg(op, pc, flags);
            t_printf(", 0x%04X", imm16);
            }
        }
}

void decode_cop0(u32 op, u32 pc, u32 flags) {
    struct cop0_decode_table_entry *ent;

    u32 func = (op >> 21) & 0x1F;

    ent = &cop0_decode_table[func];

    if(ent->mnem)
        t_printf("%-12s", ent->mnem);

    if(ent->chain)
        ent->chain(op, pc, ent->flags);
}

const char cacheOps[32][8] = {
    "IXLTG", "IXLDT", "BXLBT", "0x03", "IXSTG", "IXSDT", "BXSBT", "IXIN",
    "0x08", "0x09", "BHINBT", "IHIN", "BFH", "0x0D", "IFL", "0x0F", "DXLTG",
    "DXLDT", "DXSTG", "DXSDT", "DXWBIN", "0x15", "DXIN", "0x17", "DHWBIN",
    "0x19", "DHIN", "0x1B", "DHWOIN", "0x1D", "0x1E", "0x1F" };

void decode_cacheop(u32 op, u32 pc, u32 flags) {
    t_printf(" %s, 0x%04X(%s)", cacheOps[I_PARAM_1(op)], I_IMM16(op), genRegNames[I_PARAM_0(op)]);
}

void decode_pref(u32 op, u32 pc, u32 flags) {
}

void invalid_instruction(u32 op, u32 pc, u32 flags) {
    t_printf("                        .word 0x%08X", op);
}

void decode_1_reg(u32 op, u32 pc, u32 flags) {
    u32 rs = (op & 0x03E00000) >> 21;
    t_printf(" %s", genRegNames[rs]);
}

void decode_2_reg(u32 op, u32 pc, u32 flags) {
    t_printf(" %s, %s", genRegNames[I_PARAM_1(op)], genRegNames[I_PARAM_0(op)]);
}

void decode_2_reg_imm5(u32 op, u32 pc, u32 flags) {
    t_printf(" %s, %s, %d", genRegNames[I_PARAM_1(op)], genRegNames[I_PARAM_0(op)], I_PARAM_3(op));
}

void decode_3_reg(u32 op, u32 pc, u32 flags) {
    t_printf(" %s, %s, %s", genRegNames[I_PARAM_2(op)], genRegNames[I_PARAM_0(op)], genRegNames[I_PARAM_1(op)]);
}

void decode_syscall(u32 op, u32 pc, u32 flags) {
    u32 code;
    code = ((op << 6) >> 12);
    t_printf(" 0x%X", code);
}

void decode_break(u32 op, u32 pc, u32 flags) {
    u32 code;
    code = ((op << 6) >> 12);
    t_printf(" 0x%X", code);
}

void decode_sync(u32 op, u32 pc, u32 flags) {
    if(I_PARAM_3(op) >> 4)
        t_printf("sync.p");
    else
        t_printf("sync.l");
}

void disasm(u32 pc, u32 len) {
    u32 i;

    for(i = 0; i < len; i++) {
        t_printf("%08X  |  %08X   #   ", pc + (i * 4), ((unsigned int *) (pc))[i]);
        decode_opcode(((unsigned int *) (pc))[i], pc + (i * 4));
        }
}
