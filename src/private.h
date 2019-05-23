#ifndef CONIO_PRIVATE_H 
#define CONIO_PRIVATE_H 1

/**
 * Initializes ncurses dynamically. The initialization will only happen once,
 * even though it is called multiple times.
 */
void priv_init(void);

#endif
