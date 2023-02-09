#ifndef _ANSICHAR_H

#ifdef __cplusplus
extern "C" {
#endif

#define ANSI_CSI "\x1B["

#define ANSI_BS ('\x08')
#define ANSI_LF ('\x0A')
#define ANSI_CR ('\x0D')
#define ANSI_ESC ('\x1B')
#define ANSI_SP ('\x20')
#define ANSI_DEL ('\x7F')

#define ANSI_ERASE_ALL (2)

#ifdef __cplusplus
}
#endif

#endif // #ifndef _ANSICHAR_H
