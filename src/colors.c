#include <conio.h>
#include <conio2.h>
#undef getch

#define COLORS tmp_name
#include <ncurses.h>
#undef COLORS

#include "private.h"

/**
 * Converts a conio color to a ncurses color. Some colors are not available
 * in ncurses, and so their closest match is used.
 */
static short convert_color(COLORS color)
{
    switch (color) {
        case BLACK:
        case DARKGRAY: return COLOR_BLACK;
        case WHITE:
        case LIGHTGRAY: return COLOR_WHITE;
        case YELLOW:
        case BROWN: return COLOR_YELLOW;
        case BLUE:
        case LIGHTBLUE: return COLOR_BLUE;
        case GREEN:
        case LIGHTGREEN: return COLOR_GREEN;
        case CYAN:
        case LIGHTCYAN: return COLOR_CYAN;
        case RED:
        case LIGHTRED: return COLOR_RED;
        case MAGENTA:
        case LIGHTMAGENTA: return COLOR_MAGENTA;
        default: return color;
    }
}

void textbackground(COLORS color)
{
    short fg, bg;

    priv_init();
    pair_content(1, &fg, &bg);
    bg = convert_color(color);
    init_pair(1, fg, bg);
    color_set(1, NULL);
}

void textcolor(COLORS color)
{
    short fg, bg;

    priv_init();
    pair_content(1, &fg, &bg);
    fg = convert_color(color);
    init_pair(1, fg, bg);
    color_set(1, NULL);
}
