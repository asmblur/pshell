/*

VT100-compatible terminal

*/

#include <tamtypes.h>
#include <stdio.h>
#include "ee_sio.h"
#include "terminal.h"
#include "ansichar.h"

void VT100_EraseScreen(int mode) { tfp_printf(ANSI_CSI "%dJ", mode); }
void VT100_EraseLine(int mode) { tfp_printf(ANSI_CSI "%dK", mode); }

void VT100_SaveCursor(void) { ee_sio_putsn("\x1B" "7"); } //sio_puts(ANSI_CSI "s"); }
void VT100_RestCursor(void) { ee_sio_putsn("\x1B" "8"); } //sio_puts(ANSI_CSI "u"); }
void VT100_SetCursor(int line, int column) { tfp_printf(ANSI_CSI "%d;%dH", line, column); }
void VT100_SetCursRow(int row) { tfp_printf(ANSI_CSI "%da", row); }
void VT100_SetCursCol(int col) { tfp_printf(ANSI_CSI "%dG", col); }

void VT100_CursorUp(int num) { tfp_printf(ANSI_CSI "%dA", num); }
void VT100_CursorDown(int num) { tfp_printf(ANSI_CSI "%dB", num); }
void VT100_CursorRight(int num) { tfp_printf(ANSI_CSI "%dC", num); }
void VT100_CursorLeft(int num) { tfp_printf(ANSI_CSI "%dD", num); }

void VT100_ClearLine(void) { VT100_EraseLine(ANSI_ERASE_ALL); }

int VT100_getline(void)
{
    int ch;

    while(((ch = ee_sio_getc(-1)) == 0x0D) || (ch == 0x0A));

    while(1)
    {
        if(ch == ANSI_ESC)
        { // ESC char

            ch = ee_sio_getc(-1);

            if(ch == '[')
            {
                switch(ee_sio_getc(-1))
                {
                    case 'A': // UP
                        term_cursor_up(1);
                        break;
                    case 'B': // DOWN
                        term_cursor_down(1);
                        break;
                    case 'C': // RIGHT
                        term_cursor_right(1);
                        break;
                    case 'D': // LEFT
                        term_cursor_left(1);
                        break;
                    default:
                        Kprintf("unknown escape sequence: 0x%02X\n", ch & 0xFF);
                        break;
                }
            }
            else { Kprintf("unsupported escape sequence: 0x%02X\n", ch & 0xFF); }
        }
        else if(ch == ANSI_BS) { term_bs_char(1); }
        else if(ch == ANSI_DEL) { term_del_char(1); }
        else if(ch == ANSI_CR)
        {
            // if the command line is at least 1 char long, return success.
            if(term_get_cmd_len() > 0) { return(0); }
        }
        else if(ch != ANSI_LF)
        {
            //sio_printf("ch: %02x\n", ch);

            if(term_add_char((char) ch) == 0) { ee_sio_putc(ch); }
        }

        ch = ee_sio_getc(-1);
    }

    return(-1);
}

int VT100_init(int mode)
{
    return(0);
}

int VT100_deinit(int mode)
{
    return(0);
}

void VT100_flush(void)
{
	ee_sio_flush();
}

int VT100_getc(void)
{
    return(ee_sio_getc(-1));
}

int VT100_putc(char ch)
{
    return(ee_sio_putc(ch));
}

int VT100_cursor_up(int num) { VT100_CursorUp(num); return(0); }
int VT100_cursor_down(int num) { VT100_CursorDown(num); return(0); }
int VT100_cursor_right(int num) { VT100_CursorRight(num); return(0); }
int VT100_cursor_left(int num) { VT100_CursorLeft(num); return(0); }

int VT100_erase_line(void) { VT100_EraseLine(ANSI_ERASE_ALL); return(0); }
int VT100_erase_screen(void) { VT100_EraseScreen(ANSI_ERASE_ALL); return(0); }

int VT100_curs_save(void) { VT100_SaveCursor(); return(0); }
int VT100_curs_rest(void) { VT100_RestCursor(); return(0); }

TermIF VT100_IF =
{
    "VT100",
    &VT100_init,
    &VT100_deinit,
    &VT100_flush,
    &VT100_erase_line,
    &VT100_erase_screen,
    &VT100_getc,
    &VT100_putc,
    (int (*)(char *s)) &ee_sio_putsn,
    (int (*)(const char *format, ...)) &tfp_printf,
    &VT100_getline,
    &VT100_curs_save,
    &VT100_curs_rest,
    &VT100_cursor_up,
    &VT100_cursor_down,
    &VT100_cursor_right,
    &VT100_cursor_left,
};
