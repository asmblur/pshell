#include <tamtypes.h>
#include <ee_sio.h>
#include <ee_regs.h>
#include "pshell.h"
#include "tinyprintf.h"


#ifndef PS2LIB_STR_MAX
#define PS2LIB_STR_MAX 4096
#endif

/** Use this to determine the baud divide value.  */
#define CPUCLK		294912000
/** Baud rate generator output that divided CPUCLK.  */
#define EE_SIO_LCR_SCS_VAL	(1<<5)	

u32 ee_sio_di(void)
{
    u8 ier = *R_EE_SIO_IER;
    *R_EE_SIO_IER = ier & ~(EE_SIO_IER_ERDAI | EE_SIO_IER_ELSI); // Disable EE SIO interrupts.

    return(ier);
}

void ee_sio_clri(void)
{
    *R_EE_SIO_ISR = (EE_SIO_ISR_RX_DATA | EE_SIO_ISR_TX_EMPTY | EE_SIO_ISR_RX_ERROR); // Clear all EE SIO interrupts.
}

void ee_sio_seti(u32 v)
{
    *R_EE_SIO_IER = v;
}

void __ms_delay(int n_msecs)
{
    volatile int i;

    while(n_msecs-- > 0)
    {
        // TODO: Make this a real 1msec delay!!!!
        for(i = 0; i < 10000; i++);
    }
}

int ee_sio_getc_nb(void)
{
	if(!(*R_EE_SIO_ISR & 0x0F00)) return -1;
    return *R_EE_SIO_RXFIFO;	
}

int ee_sio_getc(int timeout)
{
    int rv;

    rv = ee_sio_getc_nb();

    if((rv < 0) && (timeout != 0))
    {
        if(timeout == -1) { while((rv = ee_sio_getc_nb()) < 0); }
        else
        {
            while((rv = ee_sio_getc_nb()) < 0) { if(timeout-- == 0) { return(-1); } __ms_delay(1000); }
        }
    }

    return(rv);
}

u8 ee_sio_getc_block(void)
{
    u8 ch;

	while(!(*R_EE_SIO_ISR & 0x0F00));

    ch = *R_EE_SIO_RXFIFO;
    *R_EE_SIO_ISR = 7;

    return(ch);
}

u8 ee_sio_getb(void)
{
    u8 ch;
	while(!(*R_EE_SIO_ISR & 0x0F00));
    ch = *R_EE_SIO_RXFIFO;
    *R_EE_SIO_ISR = 7;
    return(ch);
}

int ee_sio_putb(uint8_t d)
{
	/* Block until we're ready to transmit.  */
	while (((*R_EE_SIO_ISR) & 0xf000) == 0x8000);
	*R_EE_SIO_TXFIFO = d;
	return 1;
}

static u8 ___last_sio_putc = 0;
int ee_sio_putc(char ch)
{
    int rv = 0;

    if((ch == 0x0A) && (___last_sio_putc != 0x0D))
    {
		rv = ee_sio_putc(0x0D);
	}

	/* Block until we're ready to transmit.  */
	while (((*R_EE_SIO_ISR) & 0xf000) == 0x8000);

	*R_EE_SIO_TXFIFO = ch;

    ___last_sio_putc = ch;
    return rv + 1;
}

static void __putc(void* p, char c)
{
    ee_sio_putc(c);
}

void ee_sio_init(u32 baudrate, u8 lcr_ueps, u8 lcr_upen, u8 lcr_usbl, u8 lcr_umode)
{
    ee_sio_di();
    ee_sio_clri();

	u32 brd;		/* Baud rate divisor.  */
	u8 bclk = 0;		/* Baud rate generator clock.  */

	*R_EE_SIO_LCR = EE_SIO_LCR_SCS_VAL|((lcr_ueps & 1) << 4)|((lcr_upen & 1) << 3)|
			((lcr_usbl & 1) << 2)|(lcr_umode & 1);

	/* Reset the FIFOs.  */
	*R_EE_SIO_FCR = EE_SIO_FCR_FRSTE | EE_SIO_FCR_RFRST | EE_SIO_FCR_TFRST;

	/* Enable them.  */
	*R_EE_SIO_FCR = 0;

	brd = CPUCLK / (baudrate * 256);

	while ((brd >= 256) && (++bclk < 4))
		brd /= 4;

	*R_EE_SIO_BRC = (bclk << 8) | brd;

	init_printf(NULL,__putc);
}


void ee_sio_flush(void)
{
	volatile u8 dummy;
    while ((*R_EE_SIO_ISR) & 0xf00)
		dummy = (*R_EE_SIO_RXFIFO);
}

void ee_sio_exit(void)
{
	ee_sio_di();
	ee_sio_clri();
	ee_sio_flush();
}

size_t ee_sio_write(void *buf, size_t size)
{
	u8 *p = (u8 *)buf;
	size_t i;

	for (i = 0; i < size; i++)
		ee_sio_putb(p[i]);

	return size;
}

size_t ee_sio_read(void *buf, size_t size)
{
	u8 *p = (u8 *)buf;
	size_t i;

	for (i = 0; i < size; i++) {
		p[i] = ee_sio_getb();
	}

	return i;
}

int ee_sio_puts(const char *str)
{
	int rv;

	rv = ee_sio_putsn(str);
	rv += ee_sio_putc('\n');
	return rv;
}

int ee_sio_putsn(const char *str)
{
	int rv = 0;

	while(*str)
	    rv += ee_sio_putc(*(str++));

	return rv;
}

int ee_sio_gets(char *str)
{
	char *s = str;
	int i = 0, c;

	while (1)
	{
		c = ee_sio_getc_block();
		if(c == 0x0D) continue; // ignore '\r' chars
		if(c == 0x0A) break; // break on '\n' char

		str[i++] = c;
	}

	str[i] = '\0';
	return i;
}

