#ifndef TTY_H_INCLUDED
#define TTY_H_INCLUDED

#include <stdint.h>
#include <task.h>
#define VGAHEAD 0xB8000
#define VGAWIDTH 80
#define VGAHEIGHT 25
#define MACHINE_TITLE "root@Milk:/"

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

void tty_init(struct Task* task);
void tty_right();
void tty_left();
void tty_enter();
void tty_backspace();
void update_cursor();
void tty_home();
void tty_end();
void tty_delete();
void show_logo();
void make_title(const char* str, struct Task* task, uint8_t flag);
void tty_buffer_init(struct Task* task);
void tty_new_terminal(uint32_t id);
void tty_output_char(const char c);
#endif // TTY_H_INCLUDED
