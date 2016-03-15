#include <tty.h>
#include <string.h>
#include <stdarg.h> // used in printf function
#include <asmfunc.h>
#include <stdbool.h>

const uint32_t VGAHEAD = 0xB8000;
const uint32_t VGAWIDTH = 80;
const uint32_t VGAHEIGHT = 25;
const char MACHINE_TITLE[] = "root@KaiOS:";
const uint32_t TITLE_LEN = 11;

uint8_t tty_default_color;
uint16_t* tty_buffer;
uint32_t tty_pos_start;
uint32_t tty_pos_end;
uint32_t tty_pos_cur;
bool insert_mode = true;

static void tty_screen_up();
static inline uint8_t make_color(enum Vgacolor fg, enum Vgacolor bg);
static inline uint16_t tty_retchar(char c, uint8_t color);
static void command_line_parser(const char* command);
static inline void tty_output_char(const char c);

void tty_init() {
    tty_pos_cur = 0;
    tty_pos_end = 0;
    tty_buffer = (uint16_t*) VGAHEAD;
    tty_default_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    for(uint32_t y = 0; y < VGAHEIGHT; y++)
        for(uint32_t x = 0; x < VGAWIDTH; x++) {
            uint32_t pos = y * VGAWIDTH + x;
            tty_buffer[pos] = tty_retchar(' ', tty_default_color);
        }
    printf(MACHINE_TITLE);
    tty_pos_start = tty_pos_cur;
    return;
}

// need to be modified if there is a root@KaiOS:
void tty_backspace() {
    if(tty_pos_cur > tty_pos_start) {
        for(uint32_t i = tty_pos_cur; i <= tty_pos_end; i++)
            tty_buffer[i - 1] = tty_buffer[i];
        tty_pos_cur--;
        tty_pos_end--;
    }
    return;
}

void tty_left() {
    if(tty_pos_cur > tty_pos_start)
        tty_pos_cur--;
    return;
}

void tty_right() {
    if(tty_pos_cur < tty_pos_end)
        tty_pos_cur++;
    return;
}

void tty_home() {
    tty_pos_cur = tty_pos_start;
    return;
}

void tty_end() {
    tty_pos_cur = tty_pos_end;
    return;
}

void tty_delete() {
    if(tty_pos_cur != tty_pos_end) {
        for(uint32_t i = tty_pos_cur + 1; i <= tty_pos_end; i++)
            tty_buffer[i - 1] = tty_buffer[i];
        tty_pos_end--;
    }
    return;
}

/*
void tty_up(){

}

void tty_down(){

}
*/

void tty_enter() {
    uint32_t com_len = tty_pos_end - tty_pos_start;
    if(com_len == 0) {
        printf("\n%s", MACHINE_TITLE);
        tty_pos_start = tty_pos_cur;
    } else {
        char com_buffer[com_len];
        for(uint32_t i = 0; i < com_len; i++)
            com_buffer[i] = tty_buffer[tty_pos_start + i] & 0x00ff;
        com_buffer[com_len] = '\0';
        command_line_parser(com_buffer);
    }

    return;
}

static void command_line_parser(const char* command) {
    if(streq(command, "clear")) {
        for(uint32_t y = 0; y < VGAHEIGHT; y++)
            for(uint32_t x = 0; x < VGAWIDTH; x++) {
                uint32_t pos = y * VGAWIDTH + x;
                tty_buffer[pos] = tty_retchar(' ', tty_default_color);
            }
        tty_pos_cur = 0;
        tty_pos_end = 0;
    } else { // add command for shell
        printf("\n%s: command not found\n", command);
    }
    printf(MACHINE_TITLE);
    tty_pos_start = tty_pos_cur;
    return;
}

// temporary printf function, need to be modified formally
void printf(const char* str, ...) {
    va_list arg_list;
    va_start(arg_list, str);
    uint32_t len = strlen(str);
    for(uint32_t i = 0; i < len; i++) {
        if(str[i] == '%') {
            if(i + 1 < len) {
                if(str[i + 1] == '%') {
                    tty_output_char('%');
                } else {
                    // %s,%d,%u implementation
                    const char* arg_str;
                    if(str[i + 1] == 's') { // parser string
                        arg_str = va_arg(arg_list, const char*);
                        // common code after memory management is finished
                        uint32_t inner_len = strlen(arg_str);
                        for(uint32_t j = 0; j < inner_len; j++)
                            tty_output_char(arg_str[j]);

                    } else if(str[i + 1] == 'd' || str[i + 1] == 'u') { // parser int
                        uint32_t arg_int = va_arg(arg_list, uint32_t);
                        if(str[i + 1] == 'd') {
                            if((arg_int & (1 << 31)) == 1) {
                                tty_output_char('-');
                                arg_int = ~arg_int + 1;
                            }
                        }

                        if(arg_int == 0)
                            tty_output_char('0');
                        else {
                            // save memory because length is not sure
                            uint32_t int_len = 0;
                            uint32_t int_tmp = arg_int;
                            while(int_tmp != 0) {
                                int_len++;
                                int_tmp /= 10;
                            }
                            int_tmp = arg_int;
                            // should be modified when memory management is finished
                            char int_buffer[int_len];
                            for(int32_t j = int_len - 1; j >= 0; j--) {
                                int_buffer[j] = '0' + (char)(int_tmp % 10);
                                int_tmp /= 10;
                            }
                            for(uint32_t j = 0; j < int_len; j++)
                                tty_output_char(int_buffer[j]);
                        }
                        //arg_str = parser_int(va_arg(arg_list, int));
                    }

                }
            }
            i++;
        } else
            tty_output_char(str[i]);
    }
    return;
}

void putchar(const char c) {
    tty_output_char(c);
    return;
}

static void tty_output_char(const char c) {
    if(c == '\n') {
        uint32_t cur_row = tty_pos_cur / VGAWIDTH;
        if(cur_row == VGAHEIGHT - 1)
            tty_screen_up();
        else
            cur_row++;
        tty_pos_cur = cur_row * VGAWIDTH;
        tty_pos_end = tty_pos_cur;
    } else {
        if(insert_mode) {
            if(tty_pos_cur != tty_pos_end)
                for(uint32_t i = tty_pos_end - 1; i >= tty_pos_cur; i--)
                    tty_buffer[i + 1] = tty_buffer[i];
        }

        tty_buffer[tty_pos_cur] = tty_retchar(c, tty_default_color);
        if(insert_mode || (tty_pos_cur == tty_pos_end)) {
            tty_pos_end++;
        }
        tty_pos_cur++;
        if(tty_pos_end == VGAWIDTH * VGAHEIGHT)
            tty_screen_up();
    }

    return;
}

static void tty_screen_up() {
    for(uint32_t i = 1; i < VGAHEIGHT; i++)
        for(uint32_t j = 0; j < VGAWIDTH; j++) {
            uint32_t pos = i * VGAWIDTH + j;
            uint32_t rep = (i - 1) * VGAWIDTH + j;
            tty_buffer[rep] = tty_buffer[pos];
        }
    for(uint32_t j = 0; j < VGAWIDTH; j++) {
        uint32_t pos = (VGAHEIGHT - 1) * VGAWIDTH + j;
        tty_buffer[pos] = tty_retchar(' ', tty_default_color);
    }
    tty_pos_start -= VGAWIDTH;
    tty_pos_end -= VGAWIDTH;
    tty_pos_cur -= VGAWIDTH;
    return;
}

static inline uint8_t make_color(enum Vgacolor fg, enum Vgacolor bg) {
    return fg | bg << 4;
}

static inline uint16_t tty_retchar(char c, uint8_t color) {
    return color << 8 | c;
}
