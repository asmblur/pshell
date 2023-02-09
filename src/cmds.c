#include <kernel.h>
#include <ps2_debug.h>
#include "pshell.h"
#include "terminal.h"

extern PShellCPU _pshell_cpu_ee_l1;
extern PShellCPU _pshell_cpu_ee_l2;
extern PShellCPU _pshell_cpu_iop_l1;
extern PShellCPU *term_cpu;
extern int __iop_controlled;

void print_help(void)
{
    tprintf("- %s Help -\n\n", term_cpu->name);
    tprintf("For help on a specific command, use \"help cmd\".\n\n");
    tprintf("Available Commands:\n\n");
    term_print_cmds(term_cpu);
    tprintf("\n");
}

u128 srl128(u128 val, int sa) { return(val >> sa); }
u128 sll128(u128 val, int sa) { return(val << sa); }
u128 mult128(u128 val1, u128 val2) { return(val1 * val2); }
u128 add128(u128 val1, u128 val2) { return(val1 + val2); }

extern u128 strtou128(char *str);
extern u32 get_u32(u128 val, int n);

u128 resolve_param_u128(char *param)
{
    PShellCPU_Register *reg;

    if(param[0] == '$')
    {
        if((reg = pshell_find_reg(term_cpu, &param[1])) == NULL) { return(0); }
        return(pshell_get_reg(reg));
    }
    else if(param[0] == '(')
    {
        tprintf("expressions not supported!\n");
        return(0);
    }
    else if((param[0] == '0') && (param[1] == 'x'))
    {
        return(strtou128(param));
    }

    return(strtou128(param));
}

int cmdHelp(int argc, char *argv[])
{
    PShellCmd *cmd;

    if(argc == 1) { print_help(); }
    else
    {

        if((cmd = term_find_cmd(term_cpu, argv[1])) == NULL)
        {
            tprintf("Unknown command '%s'!\n", argv[1]);
            print_help();
            return(-1);
        }

        if(cmd->usage) { tprintf("Usage: %s\n", cmd->usage); }
        else { tprintf("No help for '%s'.\n", argv[0]); }
    }

    return(1);
}

int cmdQuit(int argc, char *argv[])
{
    return(0);
}

int cmdClearScreen(int argc, char *argv[])
{
    term_clear_screen();
    return(1);
}

int cmdShowRegs(int argc, char *argv[])
{
    PShellCPU *cpu = term_cpu;

    if(argc > 1)
    {
        if((cpu = pshell_find_cpu(argv[1])) < 0) { return(-1); }
    }

    pshell_print_regs(cpu);
    return(1);
}

int cmdSetReg(int argc, char *argv[])
{
    PShellCPU *cpu = term_cpu;
    PShellCPU_Register *reg;
    int i = 0;
    vu128 val;

    if(argc == 4)
    {
        if((cpu = pshell_find_cpu(argv[1])) < 0)
        {
            tprintf("unknown cpu '%s'!\n", argv[1]);
            return(-1);
        }

        i++;
    }

    val = resolve_param_u128(argv[i + 2]);

    if((reg = pshell_find_reg(cpu, argv[i + 1])) == NULL)
    {
        tprintf("unknown register '%s'!\n", argv[i + 1]);
        return(-2);
    }

    switch(reg->size)
    {
        case 4:
            ((u32 *) reg->value_ptr)[0] = val;
            break;
        case 8:
            ((u64 *) reg->value_ptr)[0] = val;
            break;
        case 16:
            ((u128 *) reg->value_ptr)[0] = val;
            break;
        default:
            break;
    }

    return(1);
}

int cmdPeekU8(int argc, char *argv[])
{
    u32 addr;

    addr = resolve_param_u128(argv[1]);

    tprintf("0x%08X: 0x%02X/%d\n", addr, ((vu8 *) addr)[0], ((vu8 *) addr)[0]);

    return(1);
}

int cmdPokeU8(int argc, char *argv[])
{
    u32 addr;
    u128 val;

    addr = resolve_param_u128(argv[1]);
    val = resolve_param_u128(argv[2]);

    ((vu8 *) addr)[0] = val;

    return(1);
}

int cmdPeekU16(int argc, char *argv[])
{
    u32 addr;

    addr = resolve_param_u128(argv[1]);

    tprintf("0x%08X: 0x%04X/%d\n", addr, ((vu16 *) addr)[0], ((vu16 *) addr)[0]);

    return(1);
}

int cmdPokeU16(int argc, char *argv[])
{
    u32 addr;
    u128 val;

    addr = resolve_param_u128(argv[1]);
    val = resolve_param_u128(argv[2]);

    ((vu16 *) addr)[0] = val;

    return(1);
}

int cmdPeekU32(int argc, char *argv[])
{
    u32 addr;
    u32 val;

    addr = resolve_param_u128(argv[1]);
    val = ((vu32 *) addr)[0];

    tprintf("0x%08X: 0x%08X/%d\n", addr, val, val);

    return(1);
}

int cmdPokeU32(int argc, char *argv[])
{
    u32 addr;
    u128 val;

    addr = resolve_param_u128(argv[1]);
    val = resolve_param_u128(argv[2]);

    ((vu32 *) addr)[0] = val;

    return(1);
}

u128 u64_to_u128(u64 val);

int cmdPeekU64(int argc, char *argv[])
{
    register u128 val;
    u32 addr;

    addr = resolve_param_u128(argv[1]);

    val = u64_to_u128(((vu64 *) addr)[0]);

    tprintf("0x%08X: 0x%08X%08X\n", addr, get_u32(val, 1), get_u32(val, 0));

    return(1);
}

int cmdPokeU64(int argc, char *argv[])
{
    u32 addr;
    register u128 val;

    addr = resolve_param_u128(argv[1]);
    val = resolve_param_u128(argv[2]);

    ((u64 *) addr)[0] = val;

    return(1);
}

int cmdPeekU128(int argc, char *argv[])
{
    register u128 val;
    u32 addr;

    addr = resolve_param_u128(argv[1]);

    val = ((vu128 *) addr)[0];
    tprintf("0x%08X: 0x%08X%08X%08X%08X\n", addr, get_u32(val, 3), get_u32(val, 2), get_u32(val, 1), get_u32(val, 0));

    return(1);
}

int cmdPokeU128(int argc, char *argv[])
{
    u32 addr;
    register u128 val;

    addr = resolve_param_u128(argv[1]);
    val = resolve_param_u128(argv[2]);

    ((vu128 *) addr)[0] = val;

    return(1);
}

#define DBG_BPR (1 << 0)
#define DBG_BPW (1 << 1)

u32 get_reg_32(PShellCPU *cpu, char *rname)
{
    PShellCPU_Register *reg;
    if((reg = pshell_find_reg(cpu, rname)) == NULL) { return(-1); }
    return(((vu32 *) reg->value_ptr)[0]);
}

u64 get_reg_64(PShellCPU *cpu, char *rname)
{
    PShellCPU_Register *reg;
    if((reg = pshell_find_reg(cpu, rname)) == NULL) { return(-1); }
    return(((vu64 *) reg->value_ptr)[0]);
}

u128 get_reg_128(PShellCPU *cpu, char *rname)
{
    PShellCPU_Register *reg;
    if((reg = pshell_find_reg(cpu, rname)) == NULL) { return(-1); }
    return(((vu128 *) reg->value_ptr)[0]);
}

void set_reg_32(PShellCPU *cpu, char *rname, u32 val)
{
    PShellCPU_Register *reg;
    if((reg = pshell_find_reg(cpu, rname)) == NULL) { return; }
    ((vu32 *) reg->value_ptr)[0] = val;
}

void set_reg_64(PShellCPU *cpu, char *rname, u64 val)
{
    PShellCPU_Register *reg;
    if((reg = pshell_find_reg(cpu, rname)) == NULL) { return; }
    ((vu64 *) reg->value_ptr)[0] = val;
}

void set_reg_128(PShellCPU *cpu, char *rname, u128 val)
{
    PShellCPU_Register *reg;
    if((reg = pshell_find_reg(cpu, rname)) == NULL) { return; }
    ((vu128 *) reg->value_ptr)[0] = val;
}

u128 asdf = 0;

void _dbg_set_bpx(PShellCPU *cpu, u32 addr, u32 mask)
{
    u32 ctrl_mask = 0;

    if((cpu == &_pshell_cpu_ee_l1) || (cpu == &_pshell_cpu_ee_l2))
    {
        u32 bpc = get_reg_32(cpu, "bpc");

        set_reg_32(cpu, "iab", addr);
        set_reg_32(cpu, "iabm", mask);

        ctrl_mask |= (EE_BPC_IAE);

        bpc &= ~(EE_BPC_IAE | EE_BPC_IUE | EE_BPC_ISE | EE_BPC_IKE | EE_BPC_IXE | EE_BPC_ITE | EE_BPC_DWB | EE_BPC_DRB | EE_BPC_BED);
        bpc |= (EE_BPC_IUE | EE_BPC_ISE | EE_BPC_IKE | EE_BPC_IXE | EE_BPC_ITE | ctrl_mask);

        set_reg_32(cpu, "bpc", bpc);
    }
    else
    {
        u32 dcic = get_reg_32(cpu, "dcic");

        set_reg_32(cpu, "bpc", addr);
        set_reg_32(cpu, "bpcm", mask);

        ctrl_mask |= (IOP_DCIC_PCE);

        dcic &= ~(IOP_DCIC_PCE);
        dcic |= (IOP_DCIC_UD | IOP_DCIC_KD | IOP_DCIC_TR | IOP_DCIC_DE | ctrl_mask);

        set_reg_32(cpu, "dcic", dcic);
    }
}

void _dbg_set_bpda(PShellCPU *cpu, u32 addr, u32 mask, u32 type_mask)
{
    u32 ctrl_mask = 0;

    if((cpu == &_pshell_cpu_ee_l1) || (cpu == &_pshell_cpu_ee_l2))
    {
        u32 bpc = get_reg_32(cpu, "bpc");

        set_reg_32(cpu, "dab", addr);
        set_reg_32(cpu, "dabm", mask);

        if(type_mask & DBG_BPR) { ctrl_mask |= (EE_BPC_DRE); }
        if(type_mask & DBG_BPW) { ctrl_mask |= (EE_BPC_DWE); }

        bpc &= ~(EE_BPC_DRE | EE_BPC_DWE | EE_BPC_DUE | EE_BPC_DSE | EE_BPC_DKE | EE_BPC_DXE | EE_BPC_DTE | EE_BPC_DWB | EE_BPC_DRB | EE_BPC_BED);
        bpc |= (EE_BPC_DUE | EE_BPC_DSE | EE_BPC_DKE | EE_BPC_DXE | EE_BPC_DTE | ctrl_mask);
        set_reg_32(cpu, "bpc", bpc);
    }
    else
    {
        u32 dcic = get_reg_32(cpu, "dcic");

        set_reg_32(cpu, "bda", addr);
        set_reg_32(cpu, "bdam", mask);

        if(type_mask & DBG_BPR) { ctrl_mask |= (IOP_DCIC_DR); }
        if(type_mask & DBG_BPW) { ctrl_mask |= (IOP_DCIC_DW); }

        dcic &= ~(IOP_DCIC_DR | IOP_DCIC_DW);
        dcic |= (IOP_DCIC_UD | IOP_DCIC_KD | IOP_DCIC_TR | IOP_DCIC_DAE | IOP_DCIC_DE | ctrl_mask);
        set_reg_32(cpu, "dcic", dcic);

        tprintf("setbp: %08X %08X %08X\n", dcic, addr, mask);
    }
}

void _dbg_set_bpv(PShellCPU *cpu, u32 val, u32 mask)
{
    u32 ctrl_mask = 0;

    if((cpu == &_pshell_cpu_ee_l1) || (cpu == &_pshell_cpu_ee_l2))
    {
        u32 bpc = get_reg_32(cpu, "bpc");

        set_reg_32(cpu, "dvb", val);
        set_reg_32(cpu, "dvbm", mask);

        ctrl_mask |= (EE_BPC_DVE);

        bpc &= ~(EE_BPC_DVE | EE_BPC_DUE | EE_BPC_DSE | EE_BPC_DKE | EE_BPC_DXE | EE_BPC_DTE | EE_BPC_BED);
        bpc |= (EE_BPC_DVE | EE_BPC_DUE | EE_BPC_DSE | EE_BPC_DKE | EE_BPC_DXE | EE_BPC_DTE | ctrl_mask);
        set_reg_32(cpu, "bpc", bpc);
    }
}

void _dbg_clear_bpda(PShellCPU *cpu, u32 clear_mask)
{
    u32 ctrl_mask = 0;

    if((cpu == &_pshell_cpu_ee_l1) || (cpu == &_pshell_cpu_ee_l2))
    {
        if(clear_mask & DBG_BPR) { ctrl_mask |= EE_BPC_DRE; }
        if(clear_mask & DBG_BPW) { ctrl_mask |= EE_BPC_DWE; }

        u32 bpc = get_reg_32(cpu, "bpc");
        bpc &= ~(ctrl_mask);

        if(!(bpc & (EE_BPC_DVE | EE_BPC_DWE | EE_BPC_DRE)))
        {
            // if Data Value breakpoint not enabled, disable all Data bp bits.
            bpc &= ~(EE_BPC_DUE | EE_BPC_DSE | EE_BPC_DKE | EE_BPC_DXE | EE_BPC_DTE);
        }

        set_reg_32(cpu, "bpc", bpc);

        if(!(bpc & (EE_BPC_DWE | EE_BPC_DRE)))
        {
            set_reg_32(cpu, "dab", 0);
            set_reg_32(cpu, "dabm", 0);
        }
    }
    else
    {
        u32 dcic = get_reg_32(cpu, "dcic");

        if(clear_mask & DBG_BPR) { ctrl_mask |= IOP_DCIC_DR; }
        if(clear_mask & DBG_BPW) { ctrl_mask |= IOP_DCIC_DW; }

        dcic &= ~(ctrl_mask);

        if(!(dcic & (IOP_DCIC_DR | IOP_DCIC_DW)))
        {
            ctrl_mask |= IOP_DCIC_DAE;
            set_reg_32(cpu, "bda", 0);
            set_reg_32(cpu, "bdam", 0);
        }

        set_reg_32(cpu, "dcic", dcic);
    }
}

void _dbg_clear_bpdv(PShellCPU *cpu)
{

    u32 bpc = get_reg_32(cpu, "bpc");

    bpc &= ~(EE_BPC_DVE);

    if(!(bpc & (EE_BPC_DWE | EE_BPC_DRE)))
    {
        // if Data Read or Data Write breakpoints not enabled, disable all Data bp bits.
        bpc &= ~(EE_BPC_DUE | EE_BPC_DSE | EE_BPC_DKE | EE_BPC_DXE | EE_BPC_DTE);
    }

    set_reg_32(cpu, "bpc", bpc);
}

void _dbg_clear_bpx(PShellCPU *cpu)
{
    if((cpu == &_pshell_cpu_ee_l1) || (cpu == &_pshell_cpu_ee_l2))
    {
        u32 bpc = get_reg_32(cpu, "bpc");
        bpc &= ~(EE_BPC_IXE | EE_BPC_IUE | EE_BPC_ISE | EE_BPC_IKE | EE_BPC_IXE | EE_BPC_ITE);
        set_reg_32(cpu, "bpc", bpc);
    }
    else
    {
        u32 dcic = get_reg_32(cpu, "dcic");
        dcic &= ~(IOP_DCIC_PCE);
        set_reg_32(cpu, "bpc", 0);
        set_reg_32(cpu, "bpcm", 0);
        set_reg_32(cpu, "dcic", dcic);
    }
}

void _dbg_clear_hwbps(PShellCPU *cpu)
{
    if((cpu == &_pshell_cpu_ee_l1) || (cpu == &_pshell_cpu_ee_l2))
    {
        set_reg_32(cpu, "bpc", EE_BPC_BED);
        set_reg_32(cpu, "iab", 0);
        set_reg_32(cpu, "iabm", 0);
        set_reg_32(cpu, "dab", 0);
        set_reg_32(cpu, "dabm", 0);
        set_reg_32(cpu, "dvb", 0);
        set_reg_32(cpu, "dvbm", 0);
    }
    else
    {
        set_reg_32(cpu, "dcic", 0);
        set_reg_32(cpu, "bpc", 0);
        set_reg_32(cpu, "bpcm", 0);
        set_reg_32(cpu, "bda", 0);
        set_reg_32(cpu, "bdam", 0);
    }
}

void _dbg_clear_bpr(PShellCPU *cpu) { _dbg_clear_bpda(cpu, DBG_BPR); }
void _dbg_clear_bpw(PShellCPU *cpu) { _dbg_clear_bpda(cpu, DBG_BPW); }
void _dbg_clear_bprw(PShellCPU *cpu) { _dbg_clear_bpda(cpu, DBG_BPR | DBG_BPW); }

void _dbg_set_bpr(PShellCPU *cpu, u32 addr, u32 mask) { _dbg_set_bpda(cpu, addr, mask, DBG_BPR); }
void _dbg_set_bpw(PShellCPU *cpu, u32 addr, u32 mask) { _dbg_set_bpda(cpu, addr, mask, DBG_BPW); }
void _dbg_set_bprw(PShellCPU *cpu, u32 addr, u32 mask) { _dbg_set_bpda(cpu, addr, mask, DBG_BPR | DBG_BPW); }

int cmdClearBPs(int argc, char *argv[]) { _dbg_clear_hwbps(term_cpu); return(1); }
int cmdClearBPX(int argc, char *argv[]) { _dbg_clear_bpx(term_cpu); return(1); }
int cmdClearBPR(int argc, char *argv[]) { _dbg_clear_bpr(term_cpu); return(1); }
int cmdClearBPW(int argc, char *argv[]) { _dbg_clear_bpw(term_cpu); return(1); }
int cmdClearBPRW(int argc, char *argv[]) { _dbg_clear_bprw(term_cpu); return(1); }
int cmdClearBPV(int argc, char *argv[]) { _dbg_clear_bpdv(term_cpu); return(1); }

int cmdSetBPX(int argc, char *argv[])
{
    u32 addr;
    u32 mask = 0xFFFFFFFF;
    PShellCPU *cpu = term_cpu;

    addr = strtou128(argv[1]);

    if(argc == 3) { mask = strtou128(argv[2]); }

    _dbg_set_bpx(cpu, addr, mask);

    return(1);
}

int cmdSetBPR(int argc, char *argv[])
{
    u32 addr;
    u32 mask = 0xFFFFFFFF;
    PShellCPU *cpu = term_cpu;

    addr = strtou128(argv[1]);

    if(argc == 3) { mask = strtou128(argv[2]); }

    _dbg_set_bpr(cpu, addr, mask);

    return(1);
}

int cmdSetBPW(int argc, char *argv[])
{
    u32 addr;
    u32 mask = 0xFFFFFFFF;
    PShellCPU *cpu = term_cpu;

    addr = strtou128(argv[1]);

    if(argc == 3) { mask = strtou128(argv[2]); }

    _dbg_set_bpw(cpu, addr, mask);

    return(1);
}

int cmdSetBPRW(int argc, char *argv[])
{
    u32 addr;
    u32 mask = 0xFFFFFFFF;
    PShellCPU *cpu = term_cpu;

    addr = strtou128(argv[1]);

    if(argc == 3) { mask = strtou128(argv[2]); }

    _dbg_set_bprw(cpu, addr, mask);

    return(1);
}

int cmdSetBPV(int argc, char *argv[])
{
    u32 val;
    u32 mask = 0xFFFFFFFF;
    PShellCPU *cpu = term_cpu;

    if((cpu != &_pshell_cpu_ee_l1) && (cpu != &_pshell_cpu_ee_l2))
    {
        tprintf("data value breakpoints may only be set on the EE CPU!\n");
        return(-1);
    }

    val = strtou128(argv[1]);

    if(argc == 3) { mask = strtou128(argv[2]); }

    _dbg_set_bpv(cpu, val, mask);

    return(1);
}

int cmdClearHistory(int argc, char *argv[]) { term_clear_hist(); return(1); }

int cmdIOP(int argc, char *argv[])
{
    if(!__iop_controlled)
    {
        tprintf("Sending control command...\n");

        // if the IOP is not already under our control, signal it to give us
        //  control and return 0, which causes the terminal to close.
        // the terminal will be opened automatically when the IOP signals
        //  that it is under our control.
        ee_control_iop(1, NULL);
        return(0);
    }

    // if IOP is already under EE control, just switch prompts and return 1

    term_cpu = &_pshell_cpu_iop_l1;

    return(1);
}

int cmdEE(int argc, char *argv[])
{

    if(argv[0][2] == '1') { term_cpu = &_pshell_cpu_ee_l1; }
    else { term_cpu = &_pshell_cpu_ee_l2; }

    return(1);
}

int cmdDASM(int argc, char *argv[])
{
    disasm(strtou128(argv[1]), strtou128(argv[2]));
    return(1);
}

int cmdFlushCachesEE(int argc, char *argv[])
{
    ee_flush_caches();
    return(1);
}

void *findPattern(void *start, void *end, int step, int patLen, void *patData, void *patMask)
{
    int i;

    while((start + patLen) < end) {
        for(i = 0; i < patLen; i++)
            if((((vu8 *) (start))[i] & ((u8 *) (patMask))[i]) != (((u8 *) (patData))[i] & ((u8 *) (patMask))[i]))
                break;

        if(i >= patLen)
            return(start);

        start += step;
        }

    return(NULL);
}

int cmdFindMem(int argc, char *argv[])
{
    u32 step, len;
    u8 data[1024], mask[1024];
    u32 addr;
    int i;

    if(argc < 6)
    {
        tprintf("Syntax error!\n");
    }
    else
    {
        step = strtou128(argv[3]);
        len = strtou128(argv[4]);

        if(argc < ((len * 2) + 5))
        {
            tprintf("pattern length mismatch!\n");
        }
        else
        {
            for(i = 0; i < len; i++)
            {
                data[i] = strtou128(argv[5 + i]);
                mask[i] = strtou128(argv[5 + len + i]);
            }

            tprintf("finding...");
            if((addr = (u32) findPattern((void *) ((u32) strtou128(argv[1])), (void *) ((u32) strtou128(argv[2])), step, len, data, mask)) != 0)
            {
                tprintf("found at %p.\n", addr);
            }
            else
            {
                tprintf("not found.\n");
            }
        }
    }

    return(1);
}

int cmdFillMem(int argc, char *argv[])
{
    int i, len;
    u32 addr, val;

    addr = strtou128(argv[1]);
    val = strtou128(argv[2]);
    len = strtou128(argv[3]);

    for(i = 0; i < len; i++)
        ((u8 *) (addr))[i] = val;

    return(1);
}

// EE-only commands
PShellCmd _ee_term_cmds[] =
{
    { "help", &cmdHelp, -1, -1, "This help screen or specific function.", "help [function]" },
    { "quit", &cmdQuit, 0, 0, "Quit the monitor and remain resident.", "quit" },
    { "exit", &cmdQuit, 0, 0, "Same as quit.", "exit" },
    { "clear", &cmdClearScreen, 0, 0, "Clear the terminal screen.", "clear" },
    { "clrhist", &cmdClearHistory, 0, 0, "Clear the command line history", "clrhist" },
    { "showregs", &cmdShowRegs, 0, 1, "Display a list of registers and their values for a cpu.", "showregs [ee|iop]" },
    { "setreg", &cmdSetReg, 2, 3, "Edit a register value for a cpu.", "setreg name value" },
    { "flushee", &cmdFlushCachesEE, 0, 0, "Flush the I and D caches on the EE.\n", "flush" },
    { "peekb", &cmdPeekU8, 1, 1, "peek an 8-bit address.", "peekb addr" },
    { "pokeb", &cmdPokeU8, 2, 2, "poke an 8-bit address.", "pokeb addr value" },
    { "peekh", &cmdPeekU16, 1, 1, "peek an 16-bit address.", "peekh addr" },
    { "pokeh", &cmdPokeU16, 2, 2, "poke an 16-bit address.", "pokeh addr value" },
    { "peekw", &cmdPeekU32, 1, 1, "peek an 32-bit address.", "peekw addr" },
    { "pokew", &cmdPokeU32, 2, 2, "poke an 32-bit address.", "pokew addr value" },
    { "peekd", &cmdPeekU64, 1, 1, "peek an 64-bit address.", "peekd addr" },
    { "poked", &cmdPokeU64, 2, 2, "poke an 64-bit address.", "poked addr value" },
    { "peekq", &cmdPeekU128, 1, 1, "peek an 128-bit address.", "peekq addr" },
    { "pokeq", &cmdPokeU128, 2, 2, "poke an 128-bit address.", "pokeq addr value" },

    { "setbpx", &cmdSetBPX, 1, 2, "Set a hardware breakpoint on execution.", "setbpx addr [mask]" },
    { "setbpr", &cmdSetBPR, 1, 2, "Set a hardware breakpoint on data read.", "setbpr addr [mask]" },
    { "setbpw", &cmdSetBPW, 1, 2, "Set a hardware breakpoint on data write.", "setbpw addr [mask]" },
    { "setbprw", &cmdSetBPRW, 1, 2, "Set a hardware breakpoint on data read or write.", "setbprw addr [mask]" },
    { "setbpv", &cmdSetBPV, 1, 2, "Set a hardware breakpoint on data value(EE only!).", "setbpv value [mask]" },

    { "clrbps", &cmdClearBPs, 0, 0, "Clear all hardware breakpoints.", "clrbps" },
    { "clrbpx", &cmdClearBPX, 0, 0, "Clear the hardware breakpoint on execute.", "clrbpx" },
    { "clrbpr", &cmdClearBPR, 0, 0, "Clear the hardware breakpoint on data read.", "clrbpr" },
    { "clrbpw", &cmdClearBPW, 0, 0, "Clear the hardware breakpoint on data write.", "clrbpw" },
    { "clrbprw", &cmdClearBPRW, 0, 0, "Clear the hardware breakpoint on data read or write.", "clrbprw" },
    { "clrbpv", &cmdClearBPV, 0, 0, "Clear the hardware breakpoint on data value(EE only!).", "clrbpv" },

    { "dasm", &cmdDASM, 2, 2, "Disassemble a range of memory.", "dasm addr ninstr" },
    { "findmem", &cmdFindMem, -1, -1, "Search for a memory range for a pattern, using a mask.", "findmem [start] [end] [step] [length] B1 B2 .. M1 M2 .." },
    { "fillmem", &cmdFillMem, 3, 3, "Fill the contents of a range of memory with a given value.", "fillmem [address] [value] [length]" },

    { "iop", &cmdIOP, 0, 0, "Swich to IOP Mode.", "iop" },

    { NULL, NULL, 0, 0, NULL, NULL },
};

// IOP-only commands
PShellCmd _iop_term_cmds[] =
{
    { "help", &cmdHelp, -1, -1, "This help screen or specific function.", "help [function]" },
    { "quit", &cmdQuit, 0, 0, "Quit the monitor and remain resident.", "quit" },
    { "exit", &cmdQuit, 0, 0, "Same as quit.", "exit" },
    { "clear", &cmdClearScreen, 0, 0, "Clear the terminal screen.", "clear" },
    { "clrhist", &cmdClearHistory, 0, 0, "Clear the command line history", "clrhist" },
    { "showregs", &cmdShowRegs, 0, 1, "Display a list of registers and their values for a cpu.", "showregs [ee|iop]" },
    { "setreg", &cmdSetReg, 2, 3, "Edit a register value for a cpu.", "setreg name value" },
    { "flushee", &cmdFlushCachesEE, 0, 0, "Flush the I and D caches on the EE.\n", "flush" },
    { "peekb", &cmdPeekU8, 1, 1, "peek an 8-bit address.", "peekb addr" },
    { "pokeb", &cmdPokeU8, 2, 2, "poke an 8-bit address.", "pokeb addr value" },
    { "peekh", &cmdPeekU16, 1, 1, "peek an 16-bit address.", "peekh addr" },
    { "pokeh", &cmdPokeU16, 2, 2, "poke an 16-bit address.", "pokeh addr value" },
    { "peekw", &cmdPeekU32, 1, 1, "peek an 32-bit address.", "peekw addr" },
    { "pokew", &cmdPokeU32, 2, 2, "poke an 32-bit address.", "pokew addr value" },

    { "setbpx", &cmdSetBPX, 1, 2, "Set a hardware breakpoint on execution.", "setbpx addr [mask]" },
    { "setbpr", &cmdSetBPR, 1, 2, "Set a hardware breakpoint on data read.", "setbpr addr [mask]" },
    { "setbpw", &cmdSetBPW, 1, 2, "Set a hardware breakpoint on data write.", "setbpw addr [mask]" },
    { "setbprw", &cmdSetBPRW, 1, 2, "Set a hardware breakpoint on data read or write.", "setbprw addr [mask]" },

    { "clrbps", &cmdClearBPs, 0, 0, "Clear all hardware breakpoints.", "clrbps" },
    { "clrbpx", &cmdClearBPX, 0, 0, "Clear the hardware breakpoint on execute.", "clrbpx" },
    { "clrbpr", &cmdClearBPR, 0, 0, "Clear the hardware breakpoint on data read.", "clrbpr" },
    { "clrbpw", &cmdClearBPW, 0, 0, "Clear the hardware breakpoint on data write.", "clrbpw" },
    { "clrbprw", &cmdClearBPRW, 0, 0, "Clear the hardware breakpoint on data read or write.", "clrbprw" },

    { "dasm", &cmdDASM, 2, 2, "Disassemble a range of memory.", "dasm addr ninstr" },
    { "findmem", &cmdFindMem, -1, -1, "Search for a memory range for a pattern, using a mask.", "findmem [start] [end] [step] [length] B1 B2 .. M1 M2 .." },
    { "fillmem", &cmdFillMem, 3, 3, "Fill the contents of a range of memory with a given value.", "fillmem [address] [value] [length]" },

    { "ee", &cmdEE, 0, 0, "Swich to EE Mode.", "ee" },

    { NULL, NULL, 0, 0, NULL, NULL },
};
