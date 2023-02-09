#include <kernel.h>
#include <ee_debug.h>
#include <string.h>

u128 __ee_ex_l1_stack[(1024 * 4) / 16] __attribute__ ((aligned(128)));
u128 __ee_ex_l2_stack[(1024 * 4) / 16] __attribute__ ((aligned(128)));
u128 __iop_ex_l1_stack[(1024 * 4) / 16] __attribute__ ((aligned(128)));

EE_RegFrame __ee_ex_l1_frame __attribute__ ((aligned(128)));
EE_RegFrame __ee_ex_l2_frame __attribute__ ((aligned(128)));
IOP_RegFrame __iop_ex_l1_frame __attribute__ ((aligned(128)));

u128 __saved_dbg_ex_vector[8] __attribute__ ((aligned(128)));

void *_ee_dbg_ex_dispatcher = NULL;

void *ee_dbg_set_dispatcher(void *func)
{
    void *old_func = _ee_dbg_ex_dispatcher;
    _ee_dbg_ex_dispatcher = func;
    return(old_func);
}

void *ee_dbg_get_dispatcher(void) { return(_ee_dbg_ex_dispatcher); }

extern void _ee_dbg_set_bpda(u32, u32, u32);
extern void _ee_dbg_set_bpdv(u32, u32, u32);
extern void _ee_dbg_set_bpx(u32, u32, u32);

// ex: ee_dbg_set_bpr(&var, 0xFFFFFFFF, (EE_BPC_DUE | EE_BPC_DSE | EE_BPC_DKE | EE_BPC_DXE));
void ee_dbg_set_bpr(u32 addr, u32 mask, u32 user_mask) { _ee_dbg_set_bpda(addr, mask, EE_BPC_DRE | user_mask); }

// ex: ee_dbg_set_bpw(&var, 0xFFFFFFFF, (EE_BPC_DUE | EE_BPC_DSE | EE_BPC_DKE | EE_BPC_DXE));
void ee_dbg_set_bpw(u32 addr, u32 mask, u32 user_mask) { _ee_dbg_set_bpda(addr, mask, EE_BPC_DWE | user_mask); }

// ex: ee_dbg_set_bprw(&var, 0xFFFFFFFF, (EE_BPC_DUE | EE_BPC_DSE | EE_BPC_DKE | EE_BPC_DXE));
void ee_dbg_set_bprw(u32 addr, u32 mask, u32 user_mask) { _ee_dbg_set_bpda(addr, mask, EE_BPC_DRE | EE_BPC_DWE | user_mask); }

// ex: ee_dbg_set_bpv(0xDEADBEEF, 0xFFFFFFFF, (EE_BPC_DUE | EE_BPC_DSE | EE_BPC_DKE | EE_BPC_DXE));
void ee_dbg_set_bpv(u32 value, u32 mask, u32 user_mask) { _ee_dbg_set_bpdv(value, mask, user_mask); }

// ex: ee_dbg_set_bpx(&func, 0xFFFFFFFF, (EE_BPC_IUE | EE_BPC_ISE | EE_BPC_IKE | EE_BPC_IXE));
void ee_dbg_set_bpx(u32 addr, u32 mask, u32 user_mask) { _ee_dbg_set_bpx(addr, mask, user_mask); }

void ee_dbg_clr_bpda(void)
{
    u32 bpc = ee_dbg_get_bpc();

    bpc &= ~(EE_BPC_DWE | EE_BPC_DRE);

    if(!(bpc & (EE_BPC_DVE)))
    {
        // if Data Value breakpoint not enabled, disable all Data bp bits.
        bpc &= ~(EE_BPC_DUE | EE_BPC_DSE | EE_BPC_DKE | EE_BPC_DXE | EE_BPC_DTE);
    }

    ee_dbg_set_bpc(bpc);
}

void ee_dbg_clr_bpdv(void)
{
    u32 bpc = ee_dbg_get_bpc();

    bpc &= ~(EE_BPC_DVE);

    if(!(bpc & (EE_BPC_DWE | EE_BPC_DRE)))
    {
        // if Data Read or Data Write breakpoints not enabled, disable all Data bp bits.
        bpc &= ~(EE_BPC_DUE | EE_BPC_DSE | EE_BPC_DKE | EE_BPC_DXE | EE_BPC_DTE);
    }

    ee_dbg_set_bpc(bpc);
}

void ee_dbg_clr_bpx(void)
{
    u32 bpc = ee_dbg_get_bpc();
    bpc &= ~(EE_BPC_IXE | EE_BPC_IUE | EE_BPC_ISE | EE_BPC_IKE | EE_BPC_IXE | EE_BPC_ITE);
    ee_dbg_set_bpc(bpc);
}

void ee_dbg_clear(void)
{
    ee_dbg_set_bpc(EE_BPC_BED);
    ee_dbg_set_iab(0);
    ee_dbg_set_iabm(0);
    ee_dbg_set_dab(0);
    ee_dbg_set_dabm(0);
    ee_dbg_set_dvb(0);
    ee_dbg_set_dvbm(0);
}

extern void __ee_level1_ex_vector(void);
extern void __ee_level2_ex_vector(void);

void *GetLevel1Handler(int code)
{
    int i_state, u_state;
    u32 addr;
    void *rv;

    i_state = DI();
    u_state = ee_kmode_enter();

    addr = ((s16 *) (0x8000000C))[0] << 16;
    addr += ((s16 *) (0x80000018))[0];

    rv = ((void **) addr)[code];

    if(u_state) { ee_kmode_exit(); }
    if(i_state) { EI(); }

    return(rv);
}

void *_old_l1_handlers[16];

int ee_dbg_install(void)
{
    int i_state, u_state;
    int i;

    i_state = DI();
    u_state = ee_kmode_enter();

    ee_dbg_clear();

    memcpy(&__saved_dbg_ex_vector, (void *) (0xA0000100), 0x80);

    // replace the level 2 debug exception vector with our own
    memcpy((void *) (0xA0000100), &__ee_level2_ex_vector, 0x80);

//    ee_flush_caches();

    if(u_state) { ee_kmode_exit(); }
    if(i_state) { EI(); }

// back up the existing "Level 1" exception handlers.
    for(i = 0; i < 16; i++) { _old_l1_handlers[i] = GetLevel1Handler(i); }

#if 1
    // redirect desirable "Level 1" exceptions to our level 1 "vector".
    for (i = 1; i < 4; i++) { SetVTLBRefillHandler(i, __ee_level1_ex_vector); }
    for (i = 4; i < 8; i++) { SetVCommonHandler(i, __ee_level1_ex_vector); }
    for (i = 10; i < 14; i++) { SetVCommonHandler(i, __ee_level1_ex_vector); }
#endif

    ee_flush_caches();

    return(0);
}

int ee_dbg_remove(void)
{
    int i_state, u_state;
    int i;

    i_state = DI();
    u_state = ee_kmode_enter();

    ee_dbg_clear();

    memcpy((void *) (0x80000100), &__saved_dbg_ex_vector, sizeof(__saved_dbg_ex_vector));

//    ee_flush_caches();

    if(u_state) { ee_kmode_exit(); }
    if(i_state) { EI(); }

    for (i = 1; i < 4; i++) { SetVTLBRefillHandler(i, _old_l1_handlers[i]); }
    for (i = 4; i < 8; i++) { SetVCommonHandler(i, _old_l1_handlers[i]); }
    for (i = 10; i < 14; i++) { SetVCommonHandler(i, _old_l1_handlers[i]); }

//    ee_flush_caches();

    return(0);
}
