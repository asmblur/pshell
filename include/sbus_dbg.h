#ifndef _SBUS_DBG_H
#define _SBUS_DBG_H

#include <tamtypes.h>
#include <ps2_sbus.h>

#define SBUS_DBG_MAX_CMDS (32)

#define SBUS_INTR_DBG   (16)

#define SBUS_DBG_CMD_IOP_EXC (0)
#define SBUS_DBG_CMD_PUTS (1)

typedef int (*SBusCmdFunc)(vu32 *iop_params);

int sbus_dbg_init(void);
void ee_control_iop(int mode, IOP_RegFrame *reg_frame);

#endif
