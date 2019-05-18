#include <conio.h>
#include <conio2.h>
#undef getch
#undef kbhit

#include <stdlib.h>
#include <ncurses.h>

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
 * Converts a ncurses keycode to a conio keycode.
 */
static int conv_extended(int ch)
{
    switch (ch) {
        case KEY_UP: return CONIO_UP;
        case KEY_DOWN: return CONIO_DOWN;
        case KEY_LEFT: return CONIO_LEFT;
        case KEY_RIGHT: return CONIO_RIGHT;
        default: return ch;
    }
}

/**
 * Loads a character into an input buffer, if the state says it is empty.
 * Returns whether there is a character available.
 */
static int ibuf_loadch(struct ibuf *buf)
{
    if (buf->state == IBUF_EMPTY) {
        buf->ch = getch();

        if (buf->ch != ERR) {
            if (buf->ch >= 0 && buf->ch <= 0x7F) {
                buf->state = IBUF_READY;
            } else {
                buf->state = IBUF_PARTIAL;
                buf->ch = conv_extended(buf->ch);
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
 * Initializes ncurses dynamically. The initialization will only happen once,
 * even though it is called multiple times.
 */
static void init(void)
{
    static int needs_init = 1;

    if (needs_init) {
        needs_init = 0;
        initscr();
        noecho();
        raw();
        keypad(stdscr, 1);
        set_escdelay(0);
        atexit(cleanup);
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