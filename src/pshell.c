#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <ps2_sbus.h>

#include "pshell.h"
#include "terminal.h"
#include <iop_cop0_defs.h>

PShellCPU *_pshell_cpus = NULL;
extern TermIF VT100_IF;
extern PShellCPU *term_cpu;

extern void term_open(u32 cause, void *frame);

void _ee_dbg_handler_def(u32 cause, void *frame);
void _ee_dbg_sio_rx_error(u32 cause, void *frame);
void _common_dbg_ex_handler(u32 cause, void *frame);
void _iop_exception_dispatcher(u32 cause, IOP_RegFrame *frame);

DebugCB _ee_dbg_sio_rx_func = &term_open;
DebugCB _ee_dbg_sio_tx_empty_func = (DebugCB) &_ee_dbg_handler_def;
DebugCB _ee_dbg_sio_rx_error_func = (DebugCB) &_ee_dbg_sio_rx_error;

DebugCB _ee_dbg_ex_bpx_func = term_open;
DebugCB _ee_dbg_ex_bpr_func = term_open;
DebugCB _ee_dbg_ex_bpw_func = term_open;
DebugCB _ee_dbg_ex_bpv_func = term_open;

DebugCB _iop_dbg_ex_common_func = term_open;
DebugCB _iop_dbg_ex_bpx_func = term_open;
DebugCB _iop_dbg_ex_bpr_func = term_open;
DebugCB _iop_dbg_ex_bpw_func = term_open;
#if 0
const char genRegNames[32][5];
const char COP0_RegNames[32][16];
const char Debug_RegNames[8][5];
#endif
extern EE_RegFrame __ee_ex_l1_frame;
extern EE_RegFrame __ee_ex_l2_frame;
extern IOP_RegFrame __iop_ex_l1_frame;

extern PShellCmd _ee_term_cmds[];
extern PShellCmd _iop_term_cmds[];

extern int __iop_controlled;

PShellCPU _pshell_cpu_ee_l1 =
{
    NULL,
    "EE1",
    _ee_term_cmds,
    {
        { "zero", 16, &__ee_ex_l1_frame.zero },
        { "at", 16, &__ee_ex_l1_frame.at },
        { "v0", 16, &__ee_ex_l1_frame.v0 },
        { "v1", 16, &__ee_ex_l1_frame.v1 },

        { "a0", 16, &__ee_ex_l1_frame.a0 },
        { "a1", 16, &__ee_ex_l1_frame.a1 },
        { "a2", 16, &__ee_ex_l1_frame.a2 },
        { "a3", 16, &__ee_ex_l1_frame.a3 },

        { "t0", 16, &__ee_ex_l1_frame.t0 },
        { "t1", 16, &__ee_ex_l1_frame.t1 },
        { "t2", 16, &__ee_ex_l1_frame.t2 },
        { "t3", 16, &__ee_ex_l1_frame.t3 },
        { "t4", 16, &__ee_ex_l1_frame.t4 },
        { "t5", 16, &__ee_ex_l1_frame.t5 },
        { "t6", 16, &__ee_ex_l1_frame.t6 },
        { "t7", 16, &__ee_ex_l1_frame.t7 },

        { "s0", 16, &__ee_ex_l1_frame.s0 },
        { "s1", 16, &__ee_ex_l1_frame.s1 },
        { "s2", 16, &__ee_ex_l1_frame.s2 },
        { "s3", 16, &__ee_ex_l1_frame.s3 },
        { "s4", 16, &__ee_ex_l1_frame.s4 },
        { "s5", 16, &__ee_ex_l1_frame.s5 },
        { "s6", 16, &__ee_ex_l1_frame.s6 },
        { "s7", 16, &__ee_ex_l1_frame.s7 },

        { "t8", 16, &__ee_ex_l1_frame.t8 },
        { "t9", 16, &__ee_ex_l1_frame.t9 },

        { "k0", 16, &__ee_ex_l1_frame.k0 },
        { "k1", 16, &__ee_ex_l1_frame.k1 },

        { "gp", 16, &__ee_ex_l1_frame.gp },
        { "sp", 16, &__ee_ex_l1_frame.sp },
        { "fp", 16, &__ee_ex_l1_frame.fp },
        { "ra", 16, &__ee_ex_l1_frame.ra },

        { "hi", 4, &__ee_ex_l1_frame.hi },
        { "hi1", 4, &__ee_ex_l1_frame.hi1 },
        { "lo", 4, &__ee_ex_l1_frame.lo },
        { "lo1", 4, &__ee_ex_l1_frame.lo1 },
        { "sa", 4, &__ee_ex_l1_frame.sa },

        { "epc", 4, &__ee_ex_l1_frame.epc },
        { "errorepc", 4, &__ee_ex_l1_frame.errorepc },
        { "cause", 4, &__ee_ex_l1_frame.cause },
        { "badvaddr", 4, &__ee_ex_l1_frame.badvaddr },
        { "status", 4, &__ee_ex_l1_frame.status },
        { "bpc", 4, &__ee_ex_l1_frame.bpc },
        { "iab", 4, &__ee_ex_l1_frame.iab },
        { "iabm", 4, &__ee_ex_l1_frame.iabm },
        { "dab", 4, &__ee_ex_l1_frame.dab },
        { "dabm", 4, &__ee_ex_l1_frame.dabm },
        { "dvb", 4, &__ee_ex_l1_frame.dvb },
        { "dvbm", 4, &__ee_ex_l1_frame.dvbm },
        { "", 0, NULL }
    }
};

PShellCPU _pshell_cpu_ee_l2 =
{
    NULL,
    "EE2",
    _ee_term_cmds,
    {
        { "zero", 16, &__ee_ex_l2_frame.zero },
        { "at", 16, &__ee_ex_l2_frame.at },
        { "v0", 16, &__ee_ex_l2_frame.v0 },
        { "v1", 16, &__ee_ex_l2_frame.v1 },

        { "a0", 16, &__ee_ex_l2_frame.a0 },
        { "a1", 16, &__ee_ex_l2_frame.a1 },
        { "a2", 16, &__ee_ex_l2_frame.a2 },
        { "a3", 16, &__ee_ex_l2_frame.a3 },

        { "t0", 16, &__ee_ex_l2_frame.t0 },
        { "t1", 16, &__ee_ex_l2_frame.t1 },
        { "t2", 16, &__ee_ex_l2_frame.t2 },
        { "t3", 16, &__ee_ex_l2_frame.t3 },
        { "t4", 16, &__ee_ex_l2_frame.t4 },
        { "t5", 16, &__ee_ex_l2_frame.t5 },
        { "t6", 16, &__ee_ex_l2_frame.t6 },
        { "t7", 16, &__ee_ex_l2_frame.t7 },

        { "s0", 16, &__ee_ex_l2_frame.s0 },
        { "s1", 16, &__ee_ex_l2_frame.s1 },
        { "s2", 16, &__ee_ex_l2_frame.s2 },
        { "s3", 16, &__ee_ex_l2_frame.s3 },
        { "s4", 16, &__ee_ex_l2_frame.s4 },
        { "s5", 16, &__ee_ex_l2_frame.s5 },
        { "s6", 16, &__ee_ex_l2_frame.s6 },
        { "s7", 16, &__ee_ex_l2_frame.s7 },

        { "t8", 16, &__ee_ex_l2_frame.t8 },
        { "t9", 16, &__ee_ex_l2_frame.t9 },

        { "k0", 16, &__ee_ex_l2_frame.k0 },
        { "k1", 16, &__ee_ex_l2_frame.k1 },

        { "gp", 16, &__ee_ex_l2_frame.gp },
        { "sp", 16, &__ee_ex_l2_frame.sp },
        { "fp", 16, &__ee_ex_l2_frame.fp },
        { "ra", 16, &__ee_ex_l2_frame.ra },

        { "hi", 4, &__ee_ex_l2_frame.hi },
        { "hi1", 4, &__ee_ex_l2_frame.hi1 },
        { "lo", 4, &__ee_ex_l2_frame.lo },
        { "lo1", 4, &__ee_ex_l2_frame.lo1 },
        { "sa", 4, &__ee_ex_l2_frame.sa },

        { "epc", 4, &__ee_ex_l2_frame.epc },
        { "errorepc", 4, &__ee_ex_l2_frame.errorepc },
        { "cause", 4, &__ee_ex_l2_frame.cause },
        { "badvaddr", 4, &__ee_ex_l2_frame.badvaddr },
        { "status", 4, &__ee_ex_l2_frame.status },
        { "bpc", 4, &__ee_ex_l2_frame.bpc },
        { "iab", 4, &__ee_ex_l2_frame.iab },
        { "iabm", 4, &__ee_ex_l2_frame.iabm },
        { "dab", 4, &__ee_ex_l2_frame.dab },
        { "dabm", 4, &__ee_ex_l2_frame.dabm },
        { "dvb", 4, &__ee_ex_l2_frame.dvb },
        { "dvbm", 4, &__ee_ex_l2_frame.dvbm },
        { "", 0, NULL }
    }
};

PShellCPU _pshell_cpu_iop_l1 =
{
    NULL,
    "IOP",
    _iop_term_cmds,
    {
        { "zero", 4, &__iop_ex_l1_frame.zero },
        { "at", 4, &__iop_ex_l1_frame.at },
        { "v0", 4, &__iop_ex_l1_frame.v0 },
        { "v1", 4, &__iop_ex_l1_frame.v1 },

        { "a0", 4, &__iop_ex_l1_frame.a0 },
        { "a1", 4, &__iop_ex_l1_frame.a1 },
        { "a2", 4, &__iop_ex_l1_frame.a2 },
        { "a3", 4, &__iop_ex_l1_frame.a3 },

        { "t0", 4, &__iop_ex_l1_frame.t0 },
        { "t1", 4, &__iop_ex_l1_frame.t1 },
        { "t2", 4, &__iop_ex_l1_frame.t2 },
        { "t3", 4, &__iop_ex_l1_frame.t3 },
        { "t4", 4, &__iop_ex_l1_frame.t4 },
        { "t5", 4, &__iop_ex_l1_frame.t5 },
        { "t6", 4, &__iop_ex_l1_frame.t6 },
        { "t7", 4, &__iop_ex_l1_frame.t7 },

        { "s0", 4, &__iop_ex_l1_frame.s0 },
        { "s1", 4, &__iop_ex_l1_frame.s1 },
        { "s2", 4, &__iop_ex_l1_frame.s2 },
        { "s3", 4, &__iop_ex_l1_frame.s3 },
        { "s4", 4, &__iop_ex_l1_frame.s4 },
        { "s5", 4, &__iop_ex_l1_frame.s5 },
        { "s6", 4, &__iop_ex_l1_frame.s6 },
        { "s7", 4, &__iop_ex_l1_frame.s7 },

        { "t8", 4, &__iop_ex_l1_frame.t8 },
        { "t9", 4, &__iop_ex_l1_frame.t9 },

        { "k0", 4, &__iop_ex_l1_frame.k0 },
        { "k1", 4, &__iop_ex_l1_frame.k1 },

        { "gp", 4, &__iop_ex_l1_frame.gp },
        { "sp", 4, &__iop_ex_l1_frame.sp },
        { "fp", 4, &__iop_ex_l1_frame.fp },
        { "ra", 4, &__iop_ex_l1_frame.ra },

        { "hi", 4, &__iop_ex_l1_frame.hi },
        { "lo", 4, &__iop_ex_l1_frame.lo },

        { "bpc", 4, &__iop_ex_l1_frame.bpc },
        { "bda", 4, &__iop_ex_l1_frame.bda },
        { "dcic", 4, &__iop_ex_l1_frame.dcic },
        { "badvaddr", 4, &__iop_ex_l1_frame.badvaddr },
        { "bdam", 4, &__iop_ex_l1_frame.bdam },
        { "bpcm", 4, &__iop_ex_l1_frame.bpcm },
        { "status", 4, &__iop_ex_l1_frame.status },
        { "cause", 4, &__iop_ex_l1_frame.cause },
        { "epc", 4, &__iop_ex_l1_frame.epc },
        { "", 0, NULL }
    }
};

PShellCPU *pshell_find_cpu(char *name)
{
    PShellCPU *cpu, *first;

    cpu = first = _pshell_cpus;

    if(cpu)
    {
        do
        {
            if(strcasecmp(cpu->name, name) == 0) { return(cpu); }
            cpu = cpu->next;
        } while(cpu != first);
    }

    return(NULL);
}

int pshell_register_cpu(PShellCPU *cpu)
{
    PShellCPU *first, *last;

    if(pshell_find_cpu(cpu->name)) { return(-1); }

    first = _pshell_cpus;

    if(first == NULL)
    {
        _pshell_cpus = cpu;
        cpu->next = cpu;
        return(0);
    }

    last = first->next;
    while(last->next != first) { last = last->next; }

    last->next = cpu;
    cpu->next = first;

    return(0);
}

int pshell_print_regs(PShellCPU *cpu)
{
    int i;

    tprintf("\n%s Registers:\n", cpu->name);

    for(i = 0; cpu->regs[i].name[0] != '\0'; i++)
    {
        tprintf("\t%10s: ", cpu->regs[i].name);

        switch(cpu->regs[i].size)
        {
            case 4:
                tprintf("%08X\n", ((vu32 *) cpu->regs[i].value_ptr)[0]);
                break;
            case 8:
                tprintf("%08X%08X\n", ((vu32 *) cpu->regs[i].value_ptr)[1], ((vu32 *) cpu->regs[i].value_ptr)[0]);
                break;
            case 16:
                tprintf("%08X%08X%08X%08X\n",
                    ((vu32 *) cpu->regs[i].value_ptr)[3], ((vu32 *) cpu->regs[i].value_ptr)[2],
                    ((vu32 *) cpu->regs[i].value_ptr)[1], ((vu32 *) cpu->regs[i].value_ptr)[0]);
                break;
            default: break;
        }
    }

    return(0);
}

PShellCPU_Register *pshell_find_reg(PShellCPU *cpu, char *reg_name)
{
    PShellCPU_Register *reg = cpu->regs;

//    Kprintf("find_reg '%s'...\n", reg_name);

    while(reg->name[0] != '\0')
    {
        if(strcasecmp(reg->name, reg_name) == 0) { return(reg); }
        reg++;
    }

//    Kprintf("not found!\n");

    return(NULL);
}

// called when a EE_SIO "RX Error" interrupt occurs
void _ee_dbg_sio_rx_error(u32 cause, void *frame)
{
    u8 err;

    err = *((vu8 *) EE_SIO_LSR);

    if(err & EE_SIO_LSR_FE) { Kprintf("UART: Frame error."); }// err &= ~(EE_SIO_LSR_FE); }
    else if(err & EE_SIO_LSR_PE) { Kprintf("UART: Parity error."); }//err &= ~(EE_SIO_LSR_PE); }
    else if(err & EE_SIO_LSR_OE) { Kprintf("UART: Overrun error."); }//err &= ~(EE_SIO_LSR_OE); }

    *((vu8 *) EE_SIO_LSR) = err;
}

void _ee_dbg_handler_def(u32 cause, void *frame)
{
    Kprintf("dbg_interrupt: %08X\n", cause);
}

static const char *_iop_ex_causes[] =
{
    "Interrupt",
    "TLB Modification",
    "TLB Miss Load",
    "TLB Miss Store",
    "Address Error Load",
    "Address Error Store",
    "Instruction Bus Error",
    "Data Bus Error",
    "Syscall",
    "Breakpoint",
    "Reserved Instruction",
    "Coprocessor Unusable",
    "Overflow",
    "Reserved 13",
    "Reserved 14",
};

static const char *_ee_ex_causes[] =
{
    "Interrupt",
    "TLB Modification",
    "TLB Miss Load",
    "TLB Miss Store",
    "Address Error Load",
    "Address Error Store",
    "Instruction Bus Error",
    "Data Bus Error",
    "Syscall",
    "Breakpoint",
    "Reserved Instruction",
    "Coprocessor Unusable",
    "Overflow",
    "Trap",
    "Reserved 14",
};

void print_iop_mod_info(IOP_RegFrame *iframe)
{
    ioplib_mod_image *mod;

    if((mod = findModByAddr(iframe->epc)) != NULL)
    {
        if(mod->name) { tprintf("Module: %s\n", (char *) (((u32) mod->name) | 0xBC000000)); }
        tprintf("Module Offset: 0x%08X", (u32) (((void *) iframe->epc) - mod->load_addr));

        if(iframe->epc < ((u32) mod->load_addr + mod->text_size))
        {
            tprintf("(.text)\n");
        }
        else if(iframe->epc < ((u32) mod->load_addr + mod->text_size + mod->data_size))
        {
            tprintf("(.data)\n");
        }
        else { tprintf("(.bss)\n"); }
    }
}

// called when any trapped exception occurs on EE.
void _common_dbg_ex_handler(u32 cause, void *frame)
{
    u32 bpc, dcic;
    u32 stat;
    u32 old_SIO_IER;
    EE_RegFrame *eframe = (EE_RegFrame *) frame;
    IOP_RegFrame *iframe = (IOP_RegFrame *) frame;

    old_SIO_IER = ee_sio_di();

    ee_flush_caches();

    if(cause & DBG_CAUSE_SIO)
    {
        stat = *((vu8 *) EE_SIO_ISR);

        if(stat & EE_SIO_ISR_RX_DATA)
        {
            _ee_dbg_sio_rx_func(DBG_CAUSE_EE | DBG_CAUSE_SIO, frame);

            *((vu8 *) EE_SIO_ISR) = EE_SIO_ISR_RX_DATA;
        }
        else if(stat & EE_SIO_ISR_TX_EMPTY)
        {
            _ee_dbg_sio_tx_empty_func(DBG_CAUSE_EE | DBG_CAUSE_SIO, frame);

            *((vu8 *) EE_SIO_ISR) = EE_SIO_ISR_TX_EMPTY;
        }
        else if(stat & EE_SIO_ISR_RX_ERROR)
        {
            _ee_dbg_sio_rx_error_func(DBG_CAUSE_EE | DBG_CAUSE_SIO, frame);

            *((vu8 *) EE_SIO_ISR) = EE_SIO_ISR_RX_ERROR;
        }
        else
        {
            Kprintf("unknown EE_SIO interrupt cause: %08X\n", stat);
        }
    }
    else if(cause & (DBG_CAUSE_BPR | DBG_CAUSE_BPW | DBG_CAUSE_BPX))
    {
        tprintf("%s Breakpoint at ", term_cpu->name);

        if(cause & DBG_CAUSE_EE)
        {
            if(eframe->status & EE_STATUS_EXL)
            {
                if(eframe->cause & EE_CAUSE_BD) { tprintf("0x%08X\n", eframe->epc + 4); }
                else { tprintf("0x%08X\n", eframe->epc); }

            }
            else
            {
                if(eframe->cause & EE_CAUSE_BD2) { tprintf("0x%08X\n", eframe->errorepc + 4); }
                else { tprintf("0x%08X\n", eframe->errorepc); }
            }

            tprintf("Cause: %s\n", _ee_ex_causes[(eframe->cause >> 2) & 0x1F]);
        }
        else
        {
            if(iframe->cause & IOP_CAUSE_BD) { tprintf("0x%08X\n", iframe->epc + 4); }
            else { tprintf("0x%08X\n", iframe->epc); }
            print_iop_mod_info(iframe);
            tprintf("Cause: %s\n", _iop_ex_causes[(iframe->cause >> 2) & 0x1F]);
        }

        tprintf("\n");

        if(cause & DBG_CAUSE_EE)
        {
            bpc = eframe->bpc;
            if(bpc & (EE_BPC_IAB | EE_BPC_DRB | EE_BPC_DWB)) // Exception caused by EE HWBP
            {
                if(bpc & EE_BPC_IAB) { _ee_dbg_ex_bpx_func(DBG_CAUSE_EE | DBG_CAUSE_BPX, frame); }
                if(bpc & EE_BPC_DRB) { _ee_dbg_ex_bpr_func(DBG_CAUSE_EE | DBG_CAUSE_BPR, frame); }
                if(bpc & EE_BPC_DWB) { _ee_dbg_ex_bpw_func(DBG_CAUSE_EE | DBG_CAUSE_BPW, frame); }
            }
        }
        else
        {
            dcic = iframe->dcic;

            if(dcic & IOP_DCIC_DB) // Exception caused by IOP HWBP
            {
                if(dcic & IOP_DCIC_PC) { _iop_dbg_ex_bpx_func(DBG_CAUSE_IOP | DBG_CAUSE_BPX, frame); }

                if(dcic & IOP_DCIC_DA)
                {
                    if(dcic & IOP_DCIC_R) { _iop_dbg_ex_bpr_func(DBG_CAUSE_IOP | DBG_CAUSE_BPR, frame); }
                    if(dcic & IOP_DCIC_W) { _iop_dbg_ex_bpw_func(DBG_CAUSE_IOP | DBG_CAUSE_BPW, frame); }
                }
            }
            else { _iop_dbg_ex_common_func(DBG_CAUSE_IOP | DBG_CAUSE_COMMON, frame); }
        }
    }
    else
    {
        tprintf("%s Exception at ", term_cpu->name);

        if(cause & DBG_CAUSE_EE)
        {
            if(eframe->status & EE_STATUS_EXL)
            {
                if(eframe->cause & EE_CAUSE_BD) { tprintf("0x%08X\n", eframe->epc + 4); }
                else { tprintf("0x%08X\n", eframe->epc); }

            }
            else
            {
                if(eframe->cause & EE_CAUSE_BD2) { tprintf("0x%08X\n", eframe->errorepc + 4); }
                else { tprintf("0x%08X\n", eframe->errorepc); }
            }

            tprintf("Cause: %s\n", _ee_ex_causes[(eframe->cause >> 2) & 0x1F]);
        }
        else
        {
            if(iframe->cause & IOP_CAUSE_BD) { tprintf("0x%08X\n", iframe->epc + 4); }
            else { tprintf("0x%08X\n", iframe->epc); }

            print_iop_mod_info(iframe);

            tprintf("Cause: %s\n", _iop_ex_causes[(iframe->cause >> 2) & 0x1F]);
        }

        tprintf("\n");

        term_open(cause, frame);
    }

    ee_sio_seti(old_SIO_IER);

    ee_flush_caches();
}

extern void ee_control_iop(int mode, IOP_RegFrame *reg_frame);

// called when an "IOP Exception" is received.
void _iop_exception_dispatcher(u32 cause, IOP_RegFrame *frame)
{
    int _soft_break = 0;

    term_cpu = &_pshell_cpu_iop_l1;

    // was exception caused by "BREAK" ?
    if(((frame->cause & 0x3C) >> 2) == 9)
    {
        if(((u32 *) (0xBC000000 | frame->epc))[0] == 0x0000000D)
        {
            cause = 0;
            _soft_break = 1;
        }
    }
    else
    {
        cause = DBG_CAUSE_IOP | DBG_CAUSE_COMMON;
    }

    __iop_controlled = 1;
    _common_dbg_ex_handler(cause, frame);

    // skip over the break instruction.
    if(_soft_break == 1) { __iop_ex_l1_frame.epc += 4; }

    // release the IOP.
    ee_control_iop(0, &__iop_ex_l1_frame);

    __iop_controlled = 0;
}

int _ee_ex_handler(void *frame)
{
    EE_RegFrame *eframe = (EE_RegFrame *) frame;
    u32 cause = DBG_CAUSE_EE, exc2;

    if(eframe->status & EE_STATUS_ERL)
    {
        term_cpu = &_pshell_cpu_ee_l2;

        exc2 = (eframe->cause >> 16) & 7;

        if(exc2 != EE_EXC2_DBG)
        {
            tprintf("Unknown Level 2 Exception %d!\n", exc2);
            return(0);
        }

        if(eframe->cause & EE_CAUSE_SIO) { cause |= DBG_CAUSE_SIO; }
        else { cause |= DBG_CAUSE_BPR | DBG_CAUSE_BPW | DBG_CAUSE_BPX; }
    }
    else
    {
        term_cpu = &_pshell_cpu_ee_l1;
        cause |= DBG_CAUSE_COMMON;
    }

    _common_dbg_ex_handler(cause, frame);
    return(0);
}

u128 pshell_get_reg(PShellCPU_Register *reg)
{
    switch(reg->size)
    {
        case 4:
            return(((u32 *) reg->value_ptr)[0]);
            break;
        case 8:
            return(((u64 *) reg->value_ptr)[0]);
            break;
        case 16:
            return(((u128 *) reg->value_ptr)[0]);
            break;
        default:
            break;
    }

    return(0);
}

void pshell_set_reg(PShellCPU_Register *reg, u128 val)
{
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
}

#define MAJOR_VER (1)
#define MINOR_VER (1)

extern void SBUS_dbg_init(void);

int main(int argc, char **args)
{
	// initialize EE sio as 115200 8N1
	ee_sio_init(115200, 0, 0, 0, 0);

    // set the terminal interface to VT100.
    term_init(&VT100_IF);

    tprintf("PShell v%X.%02X\n", MAJOR_VER, MINOR_VER);

    pshell_register_cpu(&_pshell_cpu_ee_l1);

    pshell_register_cpu(&_pshell_cpu_ee_l2);

    pshell_register_cpu(&_pshell_cpu_iop_l1);

    // install the EE debug exception handlers.
    if(ee_dbg_install() != 0)
    {
        tprintf("Failed installing debug system!\n");
        return(-1);
    }

    // register our Debug/SIO exception dispatcher
    ee_dbg_set_dispatcher(&_ee_ex_handler);

#if 1
    if(SBUS_init() != 0) { tprintf("EE: SBUS_init() failed!\n"); SleepThread(); }
    if(SIF2_init() != 0) { tprintf("EE: SIF2_init() failed!\n"); SleepThread(); }
    if(SIF2_init_cmd() != 0) { tprintf("EE: SIF2_init_cmd() failed!\n"); SleepThread(); }
    SBUS_dbg_init();
#endif

//    FlushCache(0);
//    FlushCache(2);

	term_flush();
    ee_sio_clri();
    ee_sio_seti(EE_SIO_IER_ERDAI | EE_SIO_IER_ELSI);

#if 1
    tprintf("EE: main() thread going to sleep...\n");
    SleepThread();
#endif

    return 0;
}
