#include "regdef.h"


// Bits in EE Cop0 Breakpoint Control(BPC)

// Instruction Address breakpoint Enable
#define BPC_IAE         1 << 31

// Data Read breakpoint Enable
#define BPC_DRE         1 << 30

// Data Write breakpoint Enable
#define BPC_DWE         1 << 29

// Data Value breakpoint Enable
#define BPC_DVE         1 << 28

// Instruction address breakpoint - User mode Enable
#define BPC_IUE         1 << 26

// Instruction address breakpoint - Supervisor mode Enable
#define BPC_ISE         1 << 25

// Instruction address breakpoint - Kernel mode Enable
#define BPC_IKE         1 << 24

// Instruction address breakpoint - EXL mode Enable
#define BPC_IXE         1 << 23

// Data breakpoint - User mode Enable
#define BPC_DUE         1 << 21

// Data breakpoint - Supervisor mode Enable
#define BPC_DSE         1 << 20

// Data breakpoint - Kernel mode Enable
#define BPC_DKE         1 << 19

// Data breakpoint - EXL mode Enable
#define BPC_DXE         1 << 18

// Instruction address breakpoint - Trigger generation Enable
#define BPC_ITE         1 << 17

// Data breakpoint - Trigger generation Enable
#define BPC_DTE         1 << 16

// Breakpoint Exception Disable
#define BPC_BED         1 << 15

// Data Write Breakpoint establishment flag
#define BPC_DWB         1 << 2

// Data Read Breakpoint establishment flag
#define BPC_DRB         1 << 1

// Instruction Address Breakpoint
#define BPC_IAB         1 << 0

#define EE_CAUSE_BD        1 << 31
#define EE_CAUSE_BD2       1 << 30

// Serial I/O pending flag
#define EE_CAUSE_SIO       1 << 12

.text

.set push
.set noreorder

.ent ee_dbg_get_bpc
.global ee_dbg_get_bpc
ee_dbg_get_bpc:
                        mfbpc   v0
                        sync.p
                        jr      ra
                        nop

.end ee_dbg_get_bpc

.global ee_dbg_get_iab
.ent ee_dbg_get_iab
ee_dbg_get_iab:
                        mfiab   v0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_get_iab

.global ee_dbg_get_iabm
.ent ee_dbg_get_iabm
ee_dbg_get_iabm:
                        mfiabm   v0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_get_iabm

.global ee_dbg_get_dab
.ent ee_dbg_get_dab
ee_dbg_get_dab:
                        mfdab   v0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_get_dab

.global ee_dbg_get_dabm
.ent ee_dbg_get_dabm
ee_dbg_get_dabm:
                        mfdabm   v0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_get_dabm

.global ee_dbg_get_dvb
.ent ee_dbg_get_dvb
ee_dbg_get_dvb:
                        mfdvb   v0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_get_dvb

.global ee_dbg_get_dvbm
.ent ee_dbg_get_dvbm
ee_dbg_get_dvbm:
                        mfdvbm   v0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_get_dvbm

.global ee_dbg_set_bpc
.ent ee_dbg_set_bpc
ee_dbg_set_bpc:
                        mtbpc   a0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_set_bpc

.global ee_dbg_set_iab
.ent ee_dbg_set_iab
ee_dbg_set_iab:
                        mtiab   a0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_set_iab

.global ee_dbg_set_iabm
.ent ee_dbg_set_iabm
ee_dbg_set_iabm:
                        mtiabm   a0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_set_iabm

.global ee_dbg_set_dab
.ent ee_dbg_set_dab
ee_dbg_set_dab:
                        mtdab   a0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_set_dab

.global ee_dbg_set_dabm
.ent ee_dbg_set_dabm
ee_dbg_set_dabm:
                        mtdabm   a0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_set_dabm

.global ee_dbg_set_dvb
.ent ee_dbg_set_dvb
ee_dbg_set_dvb:
                        mtdvb   a0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_set_dvb

.global ee_dbg_set_dvbm
.ent ee_dbg_set_dvbm
ee_dbg_set_dvbm:
                        mtdvbm   a0
                        sync.p
                        jr      ra
                        nop
.end ee_dbg_set_dvbm

.global _ee_dbg_set_bpda
.ent _ee_dbg_set_bpda
_ee_dbg_set_bpda:

                        mfbpc   t0
                        li      t1, (BPC_BED)
                        mtbpc   t1
                        sync.p

                        mtdab   a0
                        mtdabm  a1
                        sync.p

                        li      t1, ~(BPC_DRE | BPC_DWE | BPC_DUE | BPC_DSE | BPC_DKE | BPC_DXE | BPC_DTE | BPC_DWB | BPC_DRB)
                        and     t0, t0, t1

                        li      t1, (BPC_DTE)
                        or      t0, t0, t1
                        or      t0, t0, a2
                        mtbpc   t0
                        sync.p
                        jr      ra
                        nop

.end _ee_dbg_set_bpda

.global _ee_dbg_set_bpdv
.ent _ee_dbg_set_bpdv
_ee_dbg_set_bpdv:

                        mfbpc   t0
                        li      t1, (BPC_BED)
                        mtbpc   t1
                        sync.p

                        mtdvb   a0
                        mtdvbm  a1
                        sync.p

                        li      t1, (BPC_DVE | BPC_DTE)
                        or      t0, t0, t1
                        or      t0, t0, a2
                        mtbpc   t0
                        sync.p
                        jr      ra
                        nop

.end _ee_dbg_set_bpdv

.global _ee_dbg_set_bpx
.ent _ee_dbg_set_bpx
_ee_dbg_set_bpx:

                        mfbpc   t0
                        li      t1, (BPC_BED)
                        mtbpc   t1
                        sync.p

                        mtiab   a0
                        mtiabm  a1
                        sync.p

                        li      t1, (BPC_IAE | BPC_ITE)
                        or      t0, t0, t1
                        or      t0, t0, a2
                        mtbpc   t0
                        sync.p
                        jr      ra
                        nop

.end _ee_dbg_set_bpx

.local _save_frame
.ent _save_frame
_save_frame:
                        sq          zero, 0x000(s0)
.set noat
                        sq          $at,  0x010(s0)
.set at
                        sq          v0,   0x020(s0)
                        sq          v1,   0x030(s0)
                        sq          a0,   0x040(s0)
                        sq          a1,   0x050(s0)
                        sq          a2,   0x060(s0)
                        sq          a3,   0x070(s0)
                        sq          t0,   0x080(s0)
                        sq          t1,   0x090(s0)
                        sq          t2,   0x0A0(s0)
                        sq          t3,   0x0B0(s0)
                        sq          t4,   0x0C0(s0)
                        sq          t5,   0x0D0(s0)
                        sq          t6,   0x0E0(s0)
                        sq          t7,   0x0F0(s0)
#                        sq          s0,   0x100(s0) # s0 already saved
                        sq          s1,   0x110(s0)
                        sq          s2,   0x120(s0)
                        sq          s3,   0x130(s0)
                        sq          s4,   0x140(s0)
                        sq          s5,   0x150(s0)
                        sq          s6,   0x160(s0)
                        sq          s7,   0x170(s0)

                        sq          t8,   0x180(s0)
                        sq          t9,   0x190(s0)

#                        sq          k0,   0x1A0(s0)
                        sq          k1,   0x1B0(s0)

                        sq          gp,   0x1C0(s0)
#                        sq          sp,   0x1D0(s0)
                        sq          fp,   0x1E0(s0)

#                        sq          ra,   0x1F0(s0)

                        mfc0        t1, $12   #SR
                        sw          t1,   0x200(s0)

                        mfc0        t1, $13   #Cause
                        sw          t1,   0x204(s0)

                        mfc0        t1, $14   #EPC
                        sw          t1,   0x208(s0)

                        mfc0        t1, $30   #ErrorEPC
                        sw          t1,   0x20C(s0)

                        mfc0        t1, $8    #BadVAddr
                        sw          t1,   0x210(s0)

                        mfhi        t1
                        sw          t1,   0x214(s0)

                        mfhi1       t1
                        sw          t1,   0x218(s0)

                        mflo        t1
                        sw          t1,   0x21C(s0)

                        mflo1       t1
                        sw          t1,   0x220(s0)

                        mfsa        t1
                        sw          t1,   0x224(s0)

                        mfbpc       t1
                        sw          t1,   0x228(s0)
                        sync.l

                        li          t1,   (1 << 15) # BPC_BED
                        mtbpc       t1

                        mfiab       t1
                        sw          t1,   0x22C(s0)

                        mfiabm      t1
                        sw          t1,   0x230(s0)

                        mfdab       t1
                        sw          t1,   0x234(s0)

                        mfdabm      t1
                        sw          t1,   0x238(s0)

                        mfdvb       t1
                        sw          t1,   0x23C(s0)

                        mfdvbm      t1
                        sw          t1,   0x240(s0)

                        jr          ra
                        nop
.end _save_frame

.global _load_frame
.ent _load_frame
_load_frame:
                        lq          zero, 0x000(s0)
.set noat
                        lq          $at,  0x010(s0)
.set at

                        lq          v0,   0x020(s0)
                        lq          v1,   0x030(s0)
                        lq          a0,   0x040(s0)
                        lq          a1,   0x050(s0)
                        lq          a2,   0x060(s0)
                        lq          a3,   0x070(s0)
                        lq          t1,   0x090(s0)
                        lq          t2,   0x0A0(s0)
                        lq          t3,   0x0B0(s0)
                        lq          t4,   0x0C0(s0)
                        lq          t5,   0x0D0(s0)
                        lq          t6,   0x0E0(s0)
                        lq          t7,   0x0F0(s0)
#                        lq          s0,   0x100(s0)
                        lq          s1,   0x110(s0)
                        lq          s2,   0x120(s0)
                        lq          s3,   0x130(s0)
                        lq          s4,   0x140(s0)
                        lq          s5,   0x150(s0)
                        lq          s6,   0x160(s0)
                        lq          s7,   0x170(s0)

                        lq          t8,   0x180(s0)
                        lq          t9,   0x190(s0)

                        lq          k0,   0x1A0(s0)
                        lq          k1,   0x1B0(s0)

                        lq          gp,   0x1C0(s0)
                        lq          sp,   0x1D0(s0)
                        lq          fp,   0x1E0(s0)
#                        lq          ra,   0x1F0(s0)

                        lw          t0,   0x200(s0)
                        mtc0        t0, $12   #SR

                        lw          t0,   0x204(s0)
                        mtc0        t0, $13   #Cause

                        lw          t0,   0x208(s0)
                        mtc0        t0, $14   #EPC

                        lw          t0,   0x20C(s0)
                        mtc0        t0, $30   #ErrorEPC

                        lw          t0,   0x210(s0)
                        mtc0        t0, $8    #BadVAddr

                        lw          t0,   0x214(s0)
                        mthi        t0

                        lw          t0,   0x218(s0)
                        mthi        t0

                        lw          t0,   0x21C(s0)
                        mtlo        t0

                        lw          t0,   0x220(s0)
                        mtlo1       t0

                        lw          t0,   0x224(s0)
                        mtsa        t0

                        lw          t0,   0x22C(s0)
                        mtiab       t0

                        lw          t0,   0x230(s0)
                        mtiabm      t0

                        lw          t0,   0x234(s0)
                        mtdab       t0

                        lw          t0,   0x238(s0)
                        mtdabm      t0

                        lw          t0,   0x23C(s0)
                        mtdvb       t0

                        lw          t0,   0x240(s0)
                        mtdvbm      t0
                        sync.p

                        lw          t0,   0x228(s0)
                        mtbpc       t0
                        sync.p

                        jr          ra
                        lq          t0,   0x080(s0)
.end _load_frame

.global __ee_ex_handler
.ent __ee_ex_handler
__ee_ex_handler:

                        jal         _save_frame
                        nop

                        la          v0,  _ee_dbg_ex_dispatcher
                        lw          v1, 0(v0)
                        beqz        v1, __ee_ex_handler_done
                        nop

                        jalr        v1
                        move        a0, s0

__ee_ex_handler_done:
                        jal         _load_frame
                        nop

                        lq          ra, 0x1F0(s0)
                        lq          s0, 0x100(s0)
                        sync.l

                        eret
                        nop
                        nop
.end __ee_ex_handler

#define EE_EX_L1_STACK_SIZE (4 * 1024)
#define EE_EX_L2_STACK_SIZE (4 * 1024)

.global __ee_level1_ex_vector
.ent __ee_level1_ex_vector
__ee_level1_ex_vector:
                        la          k0, __ee_ex_l1_frame
                        sq          s0, 0x100(k0) # save s0
                        sq          zero, 0x1A0(k0) # k0 is not preserved in l1 exceptions
                        sq          sp, 0x1D0(k0) # save sp
                        sq          ra, 0x1F0(k0) # save ra

                        la          sp, __ee_ex_l1_stack + (EE_EX_L1_STACK_SIZE)
                        la          ra, __ee_ex_handler
                        jr          ra
                        move        s0, k0
.end __ee_level1_ex_vector


.global __ee_level2_ex_vector
.ent __ee_level2_ex_vector
__ee_level2_ex_vector:
                        sq          k0, -0x20($0) # preserve k0
                        la          k0, __ee_ex_l2_frame
                        sq          s0, 0x100(k0) # save s0
                        sq          sp, 0x1D0(k0) # save sp
                        lq          sp, -0x20($0)
                        sq          sp, 0x1A0(k0) # save k0
                        sq          ra, 0x1F0(k0) # save ra

                        la          sp, __ee_ex_l2_stack + (EE_EX_L2_STACK_SIZE)
                        la          ra, __ee_ex_handler
                        jr          ra
                        move        s0, k0
.end __ee_level2_ex_vector
.balign 128

.set pop
