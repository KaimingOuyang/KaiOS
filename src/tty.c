#include <tty.h>
#include <string.h>
#include <stdarg.h>
const uint32_t VGAHEAD = 0xB8000;
const uint32_t VGAWIDTH = 80;
const uint32_t VGAHEIGHT = 25;

uint8_t tty_default_color;
uint16_t* tty_buffer;
uint32_t tty_row;
uint32_t tty_column;

static void tty_up_row();
static uint8_t make_color(enum Vgacolor fg, enum Vgacolor bg);
static uint16_t tty_retchar(char c, uint8_t color);

void tty_init() {
    tty_row = 0;
    tty_column = 0;
    tty_buffer = (uint16_t*) VGAHEAD;
    tty_default_color = make_color(COLOR_LIGHT_GREY|8,COLOR_BLACK);
    for(uint32_t y = 0; y < VGAHEIGHT; y++)
        for(uint32_t x = 0; x < VGAWIDTH; x++) {
            uint32_t pos = y * VGAWIDTH + x;
            tty_buffer[pos] = tty_retchar(' ',tty_default_color);
        }
}

// need to be modified if there is a root@KaiOS:
void tty_backspace() {
    if(tty_column == 0 && tty_row > 0) {
        tty_column = VGAWIDTH - 1;
        tty_row--;
    } else if(tty_column > 0)
        tty_column--;
    uint32_t pos = tty_row * VGAWIDTH + tty_column;
    tty_buffer[pos] = tty_retchar(' ',tty_default_color);
    return;
}

void tty_left() {
    if(tty_column != 0)
        tty_column--;
    return;
}

// temporary function, should consider its current character boundary
void tty_right() {
    if(tty_column != VGAWIDTH - 1)
        tty_column++;
    return;
}

void printf(const char* strtmp, int32_t arg1) {

    uint32_t len = 0;
    uint32_t lens = strlen(strtmp);
    char str[len+40];

    for(uint32_t i=0; i<lens; i++) {
        if(strtmp[i] != '%')
            str[len++] = strtmp[i];
        else {
            if(strtmp[i+1] == '%') {
                i++;
                str[len++] = '%';
            } else {
                if(arg1 < 0) {
                    str[len++] = '-';
                    arg1 = -arg1;
                }

                if(arg1 == 0)
                    str[len++] = '0';
                else
                    while(arg1 != 0) {
                        str[len++] = '0' + (char)(arg1%10);
                        arg1 /= 10;
                    }
                i++;
            }
        }
    }
    str[len] = '\0';
    for(uint32_t i = 0; i < len; i++) {
        uint32_t pos = tty_row * VGAWIDTH + tty_column;

        if(str[i] == '\n') {
            tty_column = 0;
            tty_row++;
        } else {
            tty_buffer[pos] = tty_retchar(str[i],tty_default_color);
            tty_column = (tty_column + 1) % VGAWIDTH;
            if(tty_column == 0)
                tty_row++;
        }
        if(tty_row == VGAHEIGHT) {
            tty_up_row();
            tty_row--;
        }
    }
}


void putchar(const char c) {
    char buffer[2] = {c,'\0'};
    printf(buffer,0);
}

static void tty_up_row() {
    for(uint32_t i=1; i<VGAHEIGHT; i++)
        for(uint32_t j=0; j<VGAWIDTH; j++) {
            uint32_t pos = i * VGAWIDTH + j;
            uint32_t rep = (i-1) * VGAWIDTH + j;
            tty_buffer[rep] = tty_buffer[pos];
        }
    for(uint32_t j=0; j<VGAWIDTH; j++) {
        uint32_t pos = (VGAHEIGHT - 1) * VGAWIDTH + j;
        tty_buffer[pos] = tty_retchar(' ',tty_default_color);
    }
}

static uint8_t make_color(enum Vgacolor fg, enum Vgacolor bg) {
    return fg | bg << 4;
}

static uint16_t tty_retchar(char c, uint8_t color) {
    return color << 8 | c;
}
