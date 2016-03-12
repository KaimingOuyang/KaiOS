#ifndef TTY_H_INCLUDED
#define TTY_H_INCLUDED

#include <stdint.h>

enum Vgacolor {
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
};

void tty_init();
void tty_right();
void tty_left();
void tty_backspace();
void printf(const char* strtmp, int32_t arg1); // whether it should be put into I/O module?
void putchar(const char c);
#endif // TTY_H_INCLUDED
