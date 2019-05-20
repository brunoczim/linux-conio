#ifndef _CONIO2_H
#define _CONIO2_H 1

#ifdef __cplusplus
extern "C" {
#endif

#define getch _getch
#define kbhit _kbhit
#define cputs _cputs
#define putch _putch
#define cprintf _cprintf

void gotoxy(int x, int y);
void putchxy(int x, int y, char ch);
void clrscr(void);

#ifdef __cplusplus
}
#endif

#endif
