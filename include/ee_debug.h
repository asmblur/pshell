#ifndef _EE_DEBUG_H
#define _EE_DEBUG_H

#include <tamtypes.h>
#include <ee_cop0_defs.h>
#include <ps2_debug.h>

#ifdef __cplusplus
extern "C" {
#endif

int ee_dbg_install(void);
int ee_dbg_remove(void);

void *ee_dbg_get_dispatcher(void);
void *ee_dbg_set_dispatcher(void *func);

u32 ee_dbg_get_bpc(void);
u32 ee_dbg_get_iab(void);
u32 ee_dbg_get_iabm(void);
u32 ee_dbg_get_dab(void);
u32 ee_dbg_get_dabm(void);
u32 ee_dbg_get_dvb(void);
u32 ee_dbg_get_dvbm(void);

void ee_dbg_set_bpc(u32);
void ee_dbg_set_iab(u32);
void ee_dbg_set_iabm(u32);
void ee_dbg_set_dab(u32);
void ee_dbg_set_dabm(u32);
void ee_dbg_set_dvb(u32);
void ee_dbg_set_dvbm(u32);

void ee_dbg_set_bpr(u32 addr, u32 mask, u32 user_mask);
void ee_dbg_set_bpw(u32 addr, u32 mask, u32 user_mask);
void ee_dbg_set_bpv(u32 value, u32 mask, u32 user_mask);
void ee_dbg_set_bpx(u32 addr, u32 mask, u32 user_mask);

void ee_dbg_clear(void);
void ee_dbg_clr_bpda(void);
void ee_dbg_clr_bpdv(void);
void ee_dbg_clr_bpx(void);

void ee_flush_caches(void);

#ifdef __cplusplus
}
#endif

#endif
