#include <conio.h>
#include <conio2.h>
#undef getch

#include <stdio.h>
#include <stdlib.h>
#define COLORS tmp_name
#include <ncurses.h>
#undef COLORS
#include <stdarg.h>

#define CONIO_UP 72
#define CONIO_DOWN 80
#define CONIO_LEFT 75
#define CONIO_RIGHT 77

#define CONIO_EXTENDED 0xE0

/**
 * A possible state for the input buffer.
 */
enum ibuf_state {
    /** The buffer is empty. A read is required. */
    IBUF_EMPTY,
    /** The buffer is filled, but 0xE0 needs to be emitted first. */
    IBUF_PARTIAL,
    /** The buffer is ready to be consumed. */
    IBUF_READY
};

/**
 * The input buffer. Translates ncurses getch to conio's.
 */
struct ibuf {
    /** State of availability for a read. */
    enum ibuf_state state;
    /** The buffered character. */
    int ch;
};

/** The input buffer for stdscr. */
static struct ibuf global_ibuf = { IBUF_EMPTY, 0 };

/**
 * Converts a ncurses keycode to a conio keycode. Conversion happens on
 * the passed pointer. Returns if a conio keycode is implemented for the
 * ncurses keycode passed.
 */
static int conv_extended(int *ch)
{
    int success = 1;
    
    switch (*ch) {
        case KEY_UP: *ch = CONIO_UP; break;
        case KEY_DOWN: *ch = CONIO_DOWN; break;
        case KEY_LEFT: *ch = CONIO_LEFT; break;
        case KEY_RIGHT: *ch = CONIO_RIGHT; break;
        default: success = 0;
    }

    return success;
}

/**
 * Loads a character into an input buffer, if the state says it is empty.
 * Returns whether there is a character available.
 */
static int ibuf_loadch(struct ibuf *buf)
{
    int iterate = buf->state == IBUF_EMPTY;

    while (iterate) {
        buf->ch = getch();
        iterate = 0;

        if (buf->ch >= 0 && buf->ch <= 0x7F) {
            buf->state = IBUF_READY;
        } else if (buf->ch != ERR) {
            if (conv_extended(&buf->ch)) {
                buf->state = IBUF_PARTIAL;
            } else {
                iterate = 1;
            }
        }
    }

    return buf->state != IBUF_EMPTY;
}

/**
 * Skips the current character. The buffer state is updated to empty.
 * Returns the skipped character.
 */
static int ibuf_nextch(struct ibuf *buf)
{
    int ret;

    ibuf_loadch(buf);

    if (buf->state == IBUF_PARTIAL) {
        buf->state = IBUF_READY;
        ret = CONIO_EXTENDED;
    } else {
        buf->state = IBUF_EMPTY;
        ret = buf->ch;
    }

    return ret;
}

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

/**
 * Initializes ncurses dynamically. The initialization will only happen once,
 * even though it is called multiple times.
 */
static void init(void)
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

/* **********************
 * Conio functions area *
 * **********************/

int _getch()
{
    init();
    nodelay(stdscr, 0);
    return ibuf_nextch(&global_ibuf);
}

int _kbhit()
{
    init();
    nodelay(stdscr, 1);
    return ibuf_loadch(&global_ibuf);
}

int _putch(int ch)
{
    int ret = EOF;

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

    init();
    getyx(stdscr, prev_y, prev_x);

    va_start(args, fmt);
    vwprintw(stdscr, fmt, args);
    refresh();
    va_end(args);

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

    init();
    if (addstr(str) == ERR) {
        ret = -1;
    }
    refresh();

    return ret;
}

void gotoxy(int x, int y)
{
    init();
    move(y, x);
    refresh();
}

void putchxy(int x, int y, char ch)
{
    init();
    mvaddch(y, x, ch);
    refresh();
}

void cputsxy(int x, int y, char *str)
{
    init();
    mvaddstr(y, x, str);
    refresh();
}

void clrscr(void)
{
    init();
    clear();
}

int wherex(void)
{
    int x, y;
    getyx(stdscr, y, x);
    return x;
}

int wherey(void)
{
    int x, y;
    getyx(stdscr, y, x);
    return y;
}

void textbackground(COLORS color)
{
    short fg, bg;

    init();
    pair_content(1, &fg, &bg);
    bg = convert_color(color);
    init_pair(1, fg, bg);
    color_set(1, NULL);
}

void textcolor(COLORS color)
{
    short fg, bg;

    init();
    pair_content(1, &fg, &bg);
    fg = convert_color(color);
    init_pair(1, fg, bg);
    color_set(1, NULL);
}
