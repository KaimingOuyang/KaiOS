#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__i686__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

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

void printf(char* str);
int strlen(char* str);
void tty_init();
uint8_t make_color(enum Vgacolor fg, enum Vgacolor bg);
uint16_t tty_retchar(char c, uint8_t color);

const size_t VGAHEAD = 0xB8000;
const size_t VGAWIDTH = 80;
const size_t VGAHEIGHT = 25;

size_t tty_row;
size_t tty_column;
uint8_t tty_default_color;
uint16_t* tty_buffer;

void KaiOS_main() {
    tty_init();
    printf("Hello World!");
}

void tty_init() {
    tty_row = 0;
    tty_column = 0;
    tty_buffer = (uint16_t*) VGAHEAD;
    tty_default_color = make_color(COLOR_LIGHT_GREY,COLOR_BLACK);
    for(size_t y = 0; y < VGAHEIGHT; y++)
        for(size_t x = 0; x < VGAWIDTH; x++) {
            size_t pos = y * VGAWIDTH + x;
            tty_buffer[pos] = tty_retchar(' ',tty_default_color);
        }
}

void printf(char* str) {
    size_t len = strlen(str);
    for(size_t i = 0;i < len;i++){
        size_t pos = tty_row * VGAWIDTH + tty_column;
        tty_buffer[pos] = tty_retchar(str[i],tty_default_color);
        tty_column = (tty_column + 1) % VGAWIDTH;
        if(!tty_column)
            tty_row++;
    }
}

uint8_t make_color(enum Vgacolor fg, enum Vgacolor bg) {
    return fg | bg << 4;
}

int strlen(char* str) {
    if(str == NULL)
        return 0;

    int cnt = 0;
    while(str[cnt]) cnt++;
    return cnt;
}

uint16_t tty_retchar(char c, uint8_t color){
    return color << 8 | c;
}

