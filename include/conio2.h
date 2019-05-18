#ifndef _CONIO2_H
#define _CONIO2_H 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONIO_NO_ALIASES
#define getch _getch
#define kbhit _kbhit
#endif

void gotoxy(int x, int y);
void putchxy(int x, int y, char ch);

#ifdef __cplusplus
}
#endif

#endif
