#include <kernel.h>
#include <stdio.h>
#include <string.h>

extern int binary_BIN_pshell_size;
extern u8 binary_BIN_pshell_start[];

int main(int argc, char *argv[])
{
    DIntr();
    ee_kmode_enter();
    memcpy((void *) PSHELL_ADDR, &binary_BIN_pshell_start, binary_BIN_pshell_size);
    ee_kmode_exit();
    EIntr();

    FlushCache(0);
    FlushCache(2);

    DIntr();
    ee_kmode_enter();
    ((void (*)(void)) (PSHELL_ADDR))();
    ee_kmode_exit();
    EIntr();

    return(0);
}
