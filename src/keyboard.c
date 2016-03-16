#include <keyboard.h>
#include <fifo.h>
#include <tty.h>
#include <asmfunc.h>
#include <stdbool.h>

bool cap_lock;
bool num_lock;
bool scr_lock;

const uint8_t KEYSTAT = 0x64;
const uint8_t KEYDATA = 0x60;
const uint8_t KEYBOARD_NOT_READY = 0x02;

extern struct BufferPool common_buffer;
extern bool insert_mode;

static uint8_t keyboard_map[0x80] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0, 'Q',
    'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0, 0, 'A', 'S', 'D',
    'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 'V', 'B',
    'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static uint8_t shift_keymap[0x80] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0, 'Q',
    'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0, 'A', 'S', 'D',
    'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B',
    'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static inline void wait_keyboard_ready();
static void set_led();

void keyboard_init() {
    wait_keyboard_ready();
    _out8(KEYDATA, 0xed);
    wait_keyboard_ready();
    _out8(KEYDATA, 0x00);
    cap_lock = false;
    num_lock = true;
    scr_lock = false;
}

void keyboard_parser(uint16_t data) {
    if(data == 0x0f)  // tab
        printf("    "); // 1 tab equals 4 spaces
    else if(data == 0x3a) { // caps lock
        cap_lock = cap_lock ^ true;
        set_led();
    } else if(data == 0x2a || data == 0x36) { // shift
        while(1) {
            while(fifo_empty(&common_buffer));

            data = fifo_get(&common_buffer);

            if(data == 0xaa || data == 0xb6)
                break;
            else {
                if(shift_keymap[data] == '%') {
                    printf("%%");
                } else
                    putchar(shift_keymap[data]);
            }
        }
    } else if(data == 0x45) { // num lock
        num_lock = num_lock ^ true;
        set_led();
    } else if(data == 0x46) { // scroll lock
        scr_lock = scr_lock ^ true;
        set_led();
    } else if(data == 0x0E) // backspace
        tty_backspace();
    else if(data == 0x1c) // enter
        tty_enter();
    else if(data == 0xe0) { // escaped key

        data = fifo_get(&common_buffer);

        if(data == 0x1c) // keypad enter
            tty_enter();
        else if(data == 0x4b) // left key
            tty_left();
        else if(data == 0x4d) // right key
            tty_right();
        else if(data == 0x52) // insert key
            insert_mode = insert_mode ^ true;
        else if(data == 0x53) // delete key
            tty_delete();
        else if(data == 0x4f) // end key
            tty_end();
        else if(data == 0x47) // home key
            tty_home();

        // continue to finish remaining up/down and page up/down
    } else if(0x47 <= data && data <= 0x52 && data != 0x4a && data != 0x4e) { // keypad number
        if(num_lock == true)
            putchar(keyboard_map[data]);
        else if(data == 0x4f) // keypad 1 end
            tty_end();
        else if(data == 0x4b) // 4 left
            tty_left();
        else if(data == 0x4d) // 6 right
            tty_right();
        else if(data == 0x47) // 7 home
            tty_home();

        // continue to finish remaining keypad number up/down and page up/down
    } else if(0x02 <= data && data <= 0x0b) { // keyboard number
        putchar(keyboard_map[data]);
    } else {
        if(65 <= keyboard_map[data] && keyboard_map[data] <= 90) { // character
            if(cap_lock == true)
                putchar(keyboard_map[data]); // uppercase
            else
                putchar(keyboard_map[data] + 32); // lowercase
        } else
            putchar(keyboard_map[data]); // remains
    }

    return;
}

static inline void wait_keyboard_ready() {
    while(1) {
        if((_in8(KEYSTAT) & KEYBOARD_NOT_READY) == 0)
            break;
    }

    return;
}

static void set_led() {
    uint8_t data = 0;

    if(scr_lock)
        data |= 1;

    if(num_lock)
        data |= 1 << 1;

    if(cap_lock)
        data |= 1 << 2;

    wait_keyboard_ready();
    _out8(KEYDATA, 0xed);
    wait_keyboard_ready();
    _out8(KEYDATA, data);
    return;
}
