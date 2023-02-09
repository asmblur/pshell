#include <tamtypes.h>
#include "iop_lib.h"

ioplib_mod_image *findModByAddr(u32 addr)
{
    ioplib_mod_image *mod = (ioplib_mod_image *) (0xBC000800);

    addr &= 0x001FFFFF;

    while(mod)
    {
        mod = (ioplib_mod_image *) (((u32) mod) | 0xBC000000);

        if((addr >= (u32) mod->load_addr) && (addr < (u32) (mod->load_addr + mod->text_size + mod->data_size + mod->bss_size))) { break; }
        mod = mod->next;
    }

    return(mod);
}

