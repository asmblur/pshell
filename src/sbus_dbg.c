#include <kernel.h>
#include <ee_sio.h>
#include <stdio.h>
#include <string.h>
#include <ps2_sbus.h>
#include "pshell.h"
#include "ee_debug.h"
#include "terminal.h"

#define SIF2_CMD_DBG_PUTS (1)
#define SIF2_CMD_DBG_EXC (2)
#define SIF2_CMD_DBG_CTRL (3)

typedef struct st_SBUS_ControlCPU_Params
{
    u32 mode; // 0 = update register frame and release, 1 = control
    IOP_RegFrame reg_frame;
} SBUS_ControlCPU_Params;

extern IOP_RegFrame __iop_ex_l1_frame;

u32 _sif2_dbg_dma_buf[256] __attribute__ ((aligned(128)));

SBUS_ControlCPU_Params _ctrl_params __attribute__ ((aligned(128)));

int __iop_controlled = 0;

// send a string to EE for output.
void ee_control_iop(int mode, IOP_RegFrame *reg_frame)
{
    if((__iop_controlled == 1) && (mode == 1)) { return; }
    if((__iop_controlled == 0) && (mode == 0)) { return; }

//    sio_printf("iop control: %d, %d\n", __iop_controlled, mode);

    _ctrl_params.mode = mode;
    memcpy(&_ctrl_params.reg_frame, reg_frame, sizeof(IOP_RegFrame));
    SIF2_send_cmd(SIF2_CMD_DBG_CTRL, &_ctrl_params, sizeof(_ctrl_params));
}

void ee_test_iop(void)
{
    SIF2_send_cmd(10, NULL, 0);
}

void _sif2_cmd_dbg_puts(SIF2_CmdPkt *cmd, void *param)
{
    int toget, left;

    if(cmd->extra)
    {

        left = cmd->extra;

        while(left > 0)
        {
            toget = left;

            if(toget >= sizeof(_sif2_dbg_dma_buf)) { toget = sizeof(_sif2_dbg_dma_buf) - 16; }

            SIF2_set_dma((u32) &_sif2_dbg_dma_buf, toget, PS2_DMA_TO_MEM);
            SIF2_sync_dma();

            ee_sio_write((char *) (((u32) _sif2_dbg_dma_buf) | 0x20000000), toget);

            left -= toget;
        }
    }
    else
    {
        ee_sio_puts((char *) ((((u32) cmd) + sizeof(SIF2_CmdPkt)) | 0x20000000));
    }
}

extern void _iop_exception_dispatcher(u32 cause, IOP_RegFrame *frame);

void _sif2_cmd_dbg_exception(SIF2_CmdPkt *cmd, void *param)
{
    IOP_RegFrame *frame;

    if(cmd->extra)
    {
        SIF2_set_dma((u32) &_sif2_dbg_dma_buf, cmd->extra_size, PS2_DMA_TO_MEM);
        SIF2_sync_dma();
        frame = (IOP_RegFrame *) (((u32) &_sif2_dbg_dma_buf) | 0x20000000);
    }
    else
    {
        frame = (IOP_RegFrame *) ((((u32) cmd) + sizeof(SIF2_CmdPkt)) | 0x20000000);
    }

    memcpy(&__iop_ex_l1_frame, frame, sizeof(IOP_RegFrame));

    _iop_exception_dispatcher(0, frame);
}

void SBUS_dbg_init(void)
{
    SIF2_set_cmd_handler(SIF2_CMD_DBG_PUTS, &_sif2_cmd_dbg_puts, NULL);
    SIF2_set_cmd_handler(SIF2_CMD_DBG_EXC, &_sif2_cmd_dbg_exception, NULL);
}
