#include <kernel.h>
#include <string.h>
#include <ps2_debug.h>

#include "pshell.h"
#include "terminal.h"

u128 _term_stack[TERM_STACK_SIZE / 16];
int _term_is_open = 0;

extern PShellCPU *_pshell_cpus;
PShellCPU *term_cpu = NULL;

//extern term_cmds _term_cmds

TermIF *term_if = NULL;

// command line history
char term_line_hist[TERM_MAX_HIST_LINES][TERM_MAX_CMD_LINE_LEN + 1];
char *term_cmd_line = NULL;

int term_hist_first = 0;
int term_hist_last = 0;

int term_cmd_len = 0;
int term_hist_char = 0;
int term_hist_pos = 0;

// get the lenght of the current command line
int term_get_cmd_len(void) { return(term_cmd_len); }

void term_flush(void)
{
    term_if->flush();
}

void term_clear_hist(void)
{
    term_hist_pos = term_hist_first = term_hist_last = 0;
    term_cmd_line = term_line_hist[0];

    // NULL-terminate the command line
    term_cmd_line[0] = '\0';
    term_cmd_len = 0;
    term_hist_char = 0;
}

void term_print_prompt(void)
{
    term_if->erase_line();
    tprintf("\r%s@PS2: ", term_cpu->name);
}

int term_prompt(void)
{
    term_if->putc('\n'); // necessary?
    term_print_prompt();
    term_if->getline();
    return(0);
}

void term_clear_screen(void)
{
    term_if->erase_screen();
    term_print_prompt();
}

int term_set_if(TermIF *new_if)
{
    if(term_if) { term_if->deinit(0); }
    term_if = new_if;
    term_if->init(0);
    return(0);
}

// advance the "last" line in the history,
void term_hist_new_line(void)
{
    // NULL terminate the "old" line, jic..
    term_cmd_line[term_cmd_len] = '\0';

    // advance the "last" history marker
    term_hist_last = ((term_hist_last + 1) % TERM_MAX_HIST_LINES);

    if(term_hist_last == term_hist_first)
    {
        // if last == first, advance first
        term_hist_first = ((term_hist_first + 1) % TERM_MAX_HIST_LINES);
    }

    term_cmd_line = term_line_hist[term_hist_last];
    term_cmd_line[0] = '\0';
    term_cmd_len = 0;
    term_hist_char = 0;
    term_hist_pos = term_hist_last;
}

void term_update(void)
{
    term_cmd_line = term_line_hist[term_hist_last];
    term_cmd_len = strlen(term_cmd_line);
    term_cmd_line[term_cmd_len] = '\0';

    term_print_prompt();
    term_if->puts(term_cmd_line);
}

int term_hist_prev_line(void)
{
    // if already at the start, fail
    if(term_hist_pos == term_hist_first) { return(-1); }

    term_hist_pos = ((term_hist_pos - 1) % TERM_MAX_HIST_LINES);

    memcpy(&term_line_hist[term_hist_last][0], &term_line_hist[term_hist_pos][0], TERM_MAX_CMD_LINE_LEN+1);

    term_update();
    term_hist_char = term_cmd_len;

    return(0);
}

int term_hist_next_line(void)
{
    if(term_hist_pos == term_hist_last) { return(-1); }
    term_hist_pos = ((term_hist_pos + 1) % TERM_MAX_HIST_LINES);

    if(term_hist_pos != term_hist_last)
    {
		memcpy(&term_line_hist[term_hist_last][0], &term_line_hist[term_hist_pos][0], TERM_MAX_CMD_LINE_LEN+1);
    }

    term_update();
    term_hist_char = term_cmd_len;

    return(0);
}

int term_hist_prev_char(void)
{
    if(term_hist_char == 0) { return(-1); }
    term_hist_char--;
    return(0);
}

int term_hist_next_char(void)
{
    if(term_hist_char >= term_cmd_len) { return(-1); }
    term_hist_char++;
    return(0);
}

int term_bs_char(int num)
{
//    tprintf("\ncur1: %d, len %d\n", term_hist_char, term_cmd_len);
    if((term_cmd_len <= 0) || (term_hist_char == 0)) { return(-1); }

    if(term_hist_char < term_cmd_len)
    {
        memcpy(&term_cmd_line[term_hist_char - 1], &term_cmd_line[term_hist_char], term_cmd_len - term_hist_char);
    }

    term_cmd_line[--term_cmd_len] = '\0';

    term_hist_prev_char();
    term_if->cursor_left(1);

    term_if->curs_save();
    term_update();
    term_if->curs_rest();

//    tprintf("\ncur2: %d, len %d\n", term_hist_char, term_cmd_len);
    return(0);
}

int term_del_char(int num)
{
    if((term_cmd_len <= 0) || (term_hist_char == term_cmd_len)) { return(-1); }

    memcpy(&term_cmd_line[term_hist_char], &term_cmd_line[term_hist_char + 1], term_cmd_len - (term_hist_char + 1));

    term_cmd_line[--term_cmd_len] = '\0';

    term_if->curs_save();
    term_update();
    term_if->curs_rest();

    return(0);
}

int term_add_char(char ch)
{
    u8 c1, c2;
    int i;

    if(term_cmd_len >= TERM_MAX_CMD_LINE_LEN) { return(-1); }

    if(term_cmd_len > 0)
    {
        if(term_hist_char < term_cmd_len)
        {
//            sio_printf("\n%d, %d, %s => ", term_hist_char, term_cmd_len, term_cmd_line);

            i = term_hist_char;
            c2 = term_cmd_line[i];
            do
            {
                c1 = c2;
                c2 = term_cmd_line[i + 1];
                term_cmd_line[i + 1] = c1;
                i++;
            } while(c1 != '\0');

//            sio_printf("%s\n", term_cmd_line);
        }
    }
    else if(ch == '\x20') { return(-1); }

    //sio_printf("insert '%c' at %d, len %d\n", ch, term_hist_char, term_cmd_len);

    term_cmd_line[++term_cmd_len] = '\0';
    term_cmd_line[term_hist_char++] = ch;

    term_if->cursor_right(1);
    term_if->curs_save();
    term_update();
    term_if->curs_rest();

    return(1);
}

int term_cursor_up(int num) { return(term_hist_prev_line()); }
int term_cursor_down(int num) { return(term_hist_next_line()); }
int term_cursor_right(int num)
{
    if(term_hist_next_char() == 0) { return(term_if->cursor_right(num)); }
    return(-1);
}
int term_cursor_left(int num)
{
    if(term_hist_prev_char() == 0) { return(term_if->cursor_left(num)); }
    return(-1);
}

int term_parse_line(const char *line, char args[TERM_MAX_CMD_ARGS][TERM_MAX_CMD_ARG_LEN + 1])
{
    int i = 0, j = 0, k = 0;
    int in_quote = 0;

    while(line[i] != '\0')
    {
        if(line[i] == '"')
        {
            if(in_quote) { in_quote = 0; args[j++][k] = '\0'; k = 0; }
            else { in_quote = 1; }
        }
        else if(!in_quote && ((line[i] == '\x20') || (line[i] == '\t')))
        {
            if(k != 0)
            {
                args[j++][k] = '\0';
                k = 0;
            }
        }
        else { args[j][k++] = line[i]; }

        i++;
    }

    if(in_quote)
    {
        tprintf("Missing closing quote for param %d!\n", j + 1);
        return(-1);
    }

    if(k > 0) { args[j++][k] = '\0'; }

    return(j);
}

PShellCmd *term_find_cmd(PShellCPU *cpu, char *name)
{
    int i;

    if(cpu->cmds == NULL) { return(NULL); }

    for(i = 0; cpu->cmds[i].name != NULL; i++)
    {
        if(strcasecmp(cpu->cmds[i].name, name) == 0) { return(&cpu->cmds[i]); }
    }

    return(NULL);
}

void term_print_cmds(PShellCPU *cpu)
{
    int i;

    if(cpu->cmds == NULL) { return; }

    for(i = 0; cpu->cmds[i].name != NULL; i++)
    {
        tprintf("%s: %s\n", cpu->cmds[i].name, cpu->cmds[i].desc);
    }
}

int term_exec_cmd(int argc, char *argv[], int *res)
{
    int rv;
    PShellCmd *cmd;

    if((cmd = term_find_cmd(term_cpu, argv[0])) == NULL) { return(-1); }
    if((cmd->min_args >= 0) && (cmd->min_args > (argc - 1))) { return(-2); }
    if((cmd->max_args >= 0) && (cmd->max_args < (argc - 1))) { return(-3); }

    rv = cmd->call(argc, argv);

    if(res) { *res = rv; }

    return(0);
}

void term_print_cmd_info(PShellCmd *cmd) { term_if->printf("%s : %s\n", cmd->name, cmd->desc); }
void term_print_cmd_usage(PShellCmd *cmd) { term_if->printf("Usage: %s\n", cmd->usage); }

void terminal(u32 cause, void *frame)
{
    int i, res = 1, rv;
    int _argc;
    char *_argv[TERM_MAX_CMD_ARGS];
    char _args[TERM_MAX_CMD_ARGS][TERM_MAX_CMD_ARG_LEN + 1];

    term_cmd_line = term_line_hist[term_hist_last];
    term_cmd_line[0] = '\0';
    term_cmd_len = 0;
    term_hist_char = 0;
    term_hist_pos = term_hist_last;

    while(res != 0)
    {
        term_prompt();
//        term_cmd_line[term_cmd_len] = '\0'; // redundant I think.
//        term_if->puts("\n");

        if((_argc = term_parse_line(term_cmd_line, _args)) <= 0)
        {
            //tprintf("Syntax error!\n");
        }
        else
        {
            for(i = 0; i < _argc; i++) { _argv[i] = _args[i]; }

            if((rv = term_exec_cmd(_argc, _argv, &res)) < 0)
            {
                if(rv == -1)
                {
                    tprintf("Unknown command '%s'!\n", _argv[0]);
                }
                else if(rv == -2)
                {
                    tprintf("Not enough arguments!\n");
                }
                else if(rv == -3)
                {
                    tprintf("Too many arguments!\n");
                }
                else { tprintf("Unknown error: %d!\n", rv); }
            }
            else if(res < 0) { tprintf("Syntax error!\n"); }
        }

        term_hist_new_line();
    }

    term_if->flush();
}

int term_init(TermIF *tif)
{
    term_set_if(tif);
    return(0);
}
