#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <tamtypes.h>
#include "pshell.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TERM_STACK_SIZE (8 * 1024)

#define TERM_MAX_TNAME_LEN (15)
#define TERM_MAX_HIST_LINES (32)
#define TERM_MAX_CMD_LINE_LEN (255)

#define TERM_MAX_CMD_ARGS (32)
#define TERM_MAX_CMD_ARG_LEN (127)

#define TERM_CPU_EE (0)
#define TERM_CPU_IOP (1)

#define tprintf(__args...) term_if->printf(__args)

typedef struct st_TermIF
{
    char name[TERM_MAX_TNAME_LEN + 1];
    int (*init)(int);
    int (*deinit)(int);
    void (*flush)(void);
    int (*erase_line) (void);
    int (*erase_screen) (void);
    int (*getc) (void);
    int (*putc)(char ch);
    int (*puts) (char *s);
    int (*printf) (const char *format, ...);
    int (*getline) (void);
    int (*curs_save) (void);
    int (*curs_rest) (void);
    int (*cursor_up) (int num);
    int (*cursor_down) (int num);
    int (*cursor_right) (int num);
    int (*cursor_left) (int num);
} TermIF;

extern TermIF *term_if;

int term_init(TermIF *tif);
int term_set_if(TermIF *new_if);
void term_install_cmn_cmds(PShellCPU *cpu);
void term_install_iop_cmds(PShellCPU *cpu);
void term_install_ee_cmds(PShellCPU *cpu);
PShellCPU *term_find_cpu(char *name);
int term_register_cpu(PShellCPU *cpu);

PShellCmd *term_find_cmd(PShellCPU *cpu, char *name);
int term_register_cmd(PShellCPU *cpu, PShellCmd *cmd);

void term_print_cmds(PShellCPU *cpu);
int term_exec_cmd(int argc, char *argv[], int *res);
void term_print_cmd_info(struct st_PShellCmd *cmd);
void term_print_cmd_usage(struct st_PShellCmd *cmd);
void terminal(u32 cause, void *frame);

void term_flush(void);
void term_clear_screen(void);
void term_clear_hist(void);
void term_hist_new_line(void);
int term_hist_prev_line(void);
int term_hist_next_line(void);
int term_hist_prev_char(void);
int term_hist_next_char(void);
int term_bs_char(int num);
int term_del_char(int num);
int term_add_char(char ch);
int term_get_cmd_len(void);

int term_cursor_up(int num);
int term_cursor_down(int num);
int term_cursor_right(int num);
int term_cursor_left(int num);

u128 or128(u128 val1, u128 val2);

#ifdef __cplusplus
}
#endif

#endif
