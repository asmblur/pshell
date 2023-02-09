#ifndef _EE_PSHELL_H
#define _EE_PSHELL_H

#include <tamtypes.h>
#include "tinyprintf.h"
#include "ee_sio.h"
#include "ee_debug.h"
//#include "iop_debug.h"
#include <iop_cop0_defs.h>
#include <ps2_debug.h>

//#include "sbus_dbg.h"
#include "iop_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_PShellCPU_Register
{
    char name[16];
    int size;
    void *value_ptr;
} PShellCPU_Register;

typedef struct st_PShellCmd
{
    char *name;
    int (*call)(int argc, char *argv[]);
    int min_args;
    int max_args;
    char *desc;
    char *usage;
} PShellCmd;

typedef struct st_PShellCPU
{
    struct st_PShellCPU *next;
    char name[32];
    PShellCmd *cmds;
    PShellCPU_Register regs[];
} PShellCPU;


typedef void (*DebugCB)(u32, void *);

// CPU cause bit
#define DBG_CAUSE_EE            (1 <<  0)
#define DBG_CAUSE_IOP           (1 <<  1)

// EE/IOP causes

// HardWare BreakPoint causes
#define DBG_CAUSE_COMMON  (1 <<  2)
#define DBG_CAUSE_BPX     (1 <<  3)
#define DBG_CAUSE_BPR     (1 <<  4)
#define DBG_CAUSE_BPW     (1 <<  5)

// EE-only causes
#define DBG_CAUSE_BPV     (1 <<  6)
#define DBG_CAUSE_SIO     (1 <<  7)

PShellCPU *pshell_find_cpu(char *name);
PShellCPU_Register *pshell_find_reg(PShellCPU *cpu, char *reg_name);

u128 pshell_get_reg(PShellCPU_Register *reg);
void pshell_set_reg(PShellCPU_Register *reg, u128 value);
void pshell_set_reg2(u128 value, PShellCPU_Register *reg);

int pshell_print_regs(PShellCPU *cpu);

void disasm(unsigned int, unsigned int);

#define Kprintf(...) while(0){}

#ifdef __cplusplus
}
#endif

#endif // #ifndef _PSHELL_H
