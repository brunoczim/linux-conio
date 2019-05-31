#ifndef _CONIO_H
#define _CONIO_H 1

#ifdef __cplusplus
extern "C" {
#endif

int _getch(void);
int _kbhit(void);
int _putch(int ch);
int _cprintf(char const *fmt, ...);
int _cputs(char const *str);
char *_cgets(char *buf);

#ifdef __cplusplus
}
#endif

#endif
