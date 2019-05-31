#include <conio.h>
#include <conio2.h>
#undef getch

#define COLORS tmp_name
#include <ncurses.h>
#undef COLORS

#include <errno.h>
#include <stdlib.h>
#include "private.h"

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

int _getch()
{
    priv_init();
    nodelay(stdscr, 0);
    return ibuf_nextch(&global_ibuf);
}

int _kbhit()
{
    priv_init();
    nodelay(stdscr, 1);
    return ibuf_loadch(&global_ibuf);
}

char *_cgets(char *buf)
{
    int ch;
    char *last;

    priv_init();

    if (buf == NULL) {
        errno = EINVAL;
    } else {
        echo();
        last = buf;
        while ((ch = getch()) && ch != '\n') {
            *last = ch;
            last++;
        }
        *last = 0;
        noecho();
    }

    return buf;
}
