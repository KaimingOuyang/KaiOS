#include <tty.h>
#include <string.h>
const uint32_t VGAHEAD = 0xB8000;
const uint32_t VGAWIDTH = 80;
const uint32_t VGAHEIGHT = 25;

uint8_t tty_default_color;
uint16_t* tty_buffer;
uint32_t tty_row;
uint32_t tty_column;

void tty_init() {
    tty_row = 0;
    tty_column = 0;
    tty_buffer = (uint16_t*) VGAHEAD;
    tty_default_color = make_color(COLOR_LIGHT_GREY,COLOR_BLACK);
    for(uint32_t y = 0; y < VGAHEIGHT; y++)
        for(uint32_t x = 0; x < VGAWIDTH; x++) {
            uint32_t pos = y * VGAWIDTH + x;
            tty_buffer[pos] = tty_retchar(' ',tty_default_color);
        }
}

void printf(char* str) {
    uint32_t len = strlen(str);
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

uint8_t make_color(enum Vgacolor fg, enum Vgacolor bg) {
    return fg | bg << 4;
}

uint16_t tty_retchar(char c, uint8_t color) {
    return color << 8 | c;
}

void tty_up_row(){
    for(uint32_t i=1;i<VGAHEIGHT;i++)
        for(uint32_t j=0;j<VGAWIDTH;j++){
            uint32_t pos = i * VGAWIDTH + j;
            uint32_t rep = (i-1) * VGAWIDTH + j;
            tty_buffer[rep] = tty_buffer[pos];
        }
    for(uint32_t j=0;j<VGAWIDTH;j++){
        uint32_t pos = (VGAHEIGHT - 1) * VGAWIDTH + j;
        tty_buffer[pos] = tty_retchar(' ',tty_default_color);
    }
}
