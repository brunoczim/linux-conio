#include <conio.h>
#include <conio2.h>
#undef getch

#include <stdio.h>
#define COLORS tmp_name
#include <ncurses.h>
#undef COLORS
#include <stdarg.h>

#include "private.h"

int _putch(int ch)
{
    int ret = EOF;

    priv_init();
    if (addch(ch) != ERR) {
        refresh();
        ret = ch;
    }

    return ret;
}

int _cprintf(char const *fmt, ...)
{
    int ret;
    int prev_x, prev_y;
    int cur_x, cur_y;
    int max_x, max_y;
    va_list args;

    priv_init();
    getyx(stdscr, prev_y, prev_x);

    va_start(args, fmt);
    vwprintw(stdscr, fmt, args);
    va_end(args);
    refresh();

    getyx(stdscr, cur_y, cur_x);

    /* vwprintw does not return the number of written, but _cprintf does
     * We have to approximate it, although it might be wrong.
     */
    if (cur_y == prev_y) {
        ret = cur_x - prev_x;
    } else {
        getmaxyx(stdscr, max_y, max_x);
        ret = max_x - prev_x + 1;
        ret += (cur_y - prev_y - 1) * (max_x + 1);
        ret += cur_x;
    }

    return ret;
}

int _cputs(char const *str)
{
    int ret = 0;

    priv_init();
    if (addstr(str) == ERR) {
        ret = -1;
    }
    refresh();

    return ret;
}

void gotoxy(int x, int y)
{
    priv_init();
    move(y, x);
    refresh();
}

void putchxy(int x, int y, char ch)
{
    priv_init();
    mvaddch(y, x, ch);
    refresh();
}

void cputsxy(int x, int y, char *str)
{
    priv_init();
    mvaddstr(y, x, str);
    refresh();
}

void clrscr(void)
{
    priv_init();
    clear();
}

int wherex(void)
{
    int x, y;
    priv_init();
    getyx(stdscr, y, x);
    return x;
}

int wherey(void)
{
    int x, y;
    priv_init();
    getyx(stdscr, y, x);
    return y;
}
