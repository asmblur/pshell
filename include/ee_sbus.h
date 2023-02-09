#ifndef _PS2_SBUS_H
#define _PS2_SBUS_H

#include <tamtypes.h>

int SBUS_init(void);
void *SBUS_add_local_intr_handler(int irq, void *handler);
void SBUS_rem_local_intr_handler(int irq);
void SBUS_interrupt_remote(int irq);

#endif // #ifndef _PS2_SBUS_H

