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
#define cgets _cgets

typedef enum {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    DARKGRAY,
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE
} COLORS;

void gotoxy(int x, int y);
void putchxy(int x, int y, char ch);
void cputsxy(int x, int y, char *str);
void clrscr(void);
int wherex(void);
int wherey(void);

void textbackground(COLORS color);
void textcolor(COLORS color);

#ifdef __cplusplus
}
#endif

#endif
