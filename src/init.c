#include <conio.h>
#include <conio2.h>

#include <stdio.h>
#define COLORS tmp_name
#include <ncurses.h>
#undef COLORS
#include <stdlib.h>

#include "private.h"

/**
 * Cleanup routine required to be executed in the end of the process,
 * otherwise resources could be leaked or the terminal emulator could
 * behave weirdly.
 */
static void cleanup(void)
{
    endwin();
}

/**
 * Handles color resources initialization. Do not call directly.
 */
static void init_colors(void)
{
    short fg, bg;

    if (!has_colors() || !can_change_color()) {
        fputs("Error: no colors available", stderr);
        cleanup();
        abort();
    }

    start_color();
    pair_content(0, &fg, &bg);
    init_pair(1, fg, bg);
}

void priv_init(void)
{
    static int needs_init = 1;

    if (needs_init) {
        needs_init = 0;
        initscr();
        init_colors();
        noecho();
        cbreak();
        keypad(stdscr, 1);
        set_escdelay(0);
        atexit(cleanup);
    }
}
