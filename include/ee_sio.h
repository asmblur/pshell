#ifndef _EE_SIO_H
#define _EE_SIO_H

#include <stdio.h>
#include "tinyprintf.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 1
/* SIO Registers.  */
/* Most of these are based off of Toshiba documentation for the TX49 and the
   TX79 companion chip. However, looking at the kernel SIOP (Debug) exception
   handler, it looks like some registers are borrowed from the TX7901 UART
   (0x1000f110 or LSR, in particular). I'm still trying to find the correct
   register names and values.  */
/** Line Control Register.  */
#define EE_SIO_LCR		0x1000f100
/** UART Mode.  */
#define   EE_SIO_LCR_UMODE_8BIT 0x00
#define   EE_SIO_LCR_UMODE_7BIT 0x01
/** UART Stop Bit Length.  */
#define   EE_SIO_LCR_USBL_1BIT  0x00
#define   EE_SIO_LCR_USBL_2BITS 0x01
/** UART Parity Enable.  */
#define   EE_SIO_LCR_UPEN_OFF   0x00
#define   EE_SIO_LCR_UPEN_ON    0x01
/** UART Even Parity Select.  */
#define   EE_SIO_LCR_UEPS_ODD   0x00
#define   EE_SIO_LCR_UEPS_EVEN  0x01

/** Line Status Register.  */
#define EE_SIO_LSR		0x1000f110
/** Data Ready. (Not tested) */
#define   EE_SIO_LSR_DR 0x01
/** Overrun Error.  */
#define   EE_SIO_LSR_OE 0x02
/** Parity Error.  */
#define   EE_SIO_LSR_PE 0x04
/** Framing Error.  */
#define   EE_SIO_LSR_FE 0x08

/** Interrupt Enable Register.  */
#define EE_SIO_IER		0x1000f120	
/** Enable Received Data Available Interrupt */
#define   EE_SIO_IER_ERDAI	0x01		
/** Enable Line Status Interrupt.  */
#define   EE_SIO_IER_ELSI 	0x04		

/** Interrupt Status Register (?).  */
#define EE_SIO_ISR		0x1000f130	
#define   EE_SIO_ISR_RX_DATA  0x01
#define   EE_SIO_ISR_TX_EMPTY 0x02
#define   EE_SIO_ISR_RX_ERROR 0x04

/** FIFO Control Register.  */
#define EE_SIO_FCR		0x1000f140
/** FIFO Reset Enable.  */
#define   EE_SIO_FCR_FRSTE 0x01
/** RX FIFO Reset.  */
#define   EE_SIO_FCR_RFRST 0x02
/** TX FIFO Reset.  */
#define   EE_SIO_FCR_TFRST 0x04

/** Baud Rate Control Register.  */
#define EE_SIO_BRC		0x1000f150

/** Transmit FIFO.  */
#define EE_SIO_TXFIFO	0x1000f180
/** Receive FIFO.  */
#define EE_SIO_RXFIFO	0x1000f1c0

/** The bit in the EE cause register which indicates an SIO exception */
#define EE_SIO_CAUSE_BIT	(1 << 12)
#endif

u32 ee_sio_di(void);
void ee_sio_clri(void);
void ee_sio_seti(u32 v);
int ee_sio_getc_nb(void);
int ee_sio_getc(int timeout);
u8 ee_sio_getc_block(void);
u8 ee_sio_getb(void);
int ee_sio_putb(uint8_t d);
int ee_sio_putc(char ch);
void ee_sio_init(u32 baudrate, u8 lcr_ueps, u8 lcr_upen, u8 lcr_usbl, u8 lcr_umode);
void ee_sio_flush(void);
void ee_sio_exit(void);
size_t ee_sio_write(void *buf, size_t size);
size_t ee_sio_read(void *buf, size_t size);
int ee_sio_puts(const char *str);
int ee_sio_putsn(const char *str);
int ee_sio_gets(char *str);


#ifdef __cplusplus
}
#endif

#endif // #ifndef _EE_SIO_H
