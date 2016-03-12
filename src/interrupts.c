#include <interrupts.h>
#include <tty.h>
#include <asmfunc.h>
#include <stdbool.h>
#include <fifo.h>

const uint32_t PIC0_ICW1 = 0x0020;
const uint32_t PIC0_ICW2 = 0x0021;
const uint32_t PIC0_ICW3 = 0x0021;
const uint32_t PIC0_ICW4 = 0x0021;

const uint32_t PIC1_ICW1 = 0x00a0;
const uint32_t PIC1_ICW2 = 0x00a1;
const uint32_t PIC1_ICW3 = 0x00a1;
const uint32_t PIC1_ICW4 = 0x00a1;

const uint32_t PIC0_IMR = 0x0021;
const uint32_t PIC1_IMR = 0x00a1;

const uint32_t PIC0_OCW2 = 0x0020;
const uint32_t PIC1_OCW2 = 0x00a0;

const uint16_t PORT_KEYBOARD = 0x0060;

void pic_init() {

    _out8(PIC0_IMR,0xff);
    _out8(PIC1_IMR,0xff);

    _out8(PIC0_ICW1,0x11);
    _out8(PIC0_ICW2,0x20);
    _out8(PIC0_ICW3,0x04);
    _out8(PIC0_ICW4,0x01);

    _out8(PIC1_ICW1,0x11);
    _out8(PIC1_ICW1,0x28);
    _out8(PIC1_ICW1,0x04);
    _out8(PIC1_ICW1,0x01);

    _out8(PIC0_IMR,0xfb);
    _out8(PIC1_IMR,0xff);
    _sti();
    return;
}


void pic_set_mask(uint8_t pic,uint16_t attr) {
    switch(pic) {
    case PIC0:
        _out8(PIC0_IMR,attr);
        break;
    case PIC1:
        _out8(PIC1_IMR,attr);
        break;
    }
    return;
}


/*
    Keyboard code
*/

const uint32_t KEYSTAT = 0x64;
const uint32_t KEYDATA = 0x60;
const uint8_t KEYBOARD_NOT_READY = 0x02;

static uint8_t keyboard_map[0x80] = {
    0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,'Q',
    'W','E','R','T','Y','U','I','O','P','[',']',0,0,'A','S','D',
    'F','G','H','J','K','L',';','\'','`',0,'\\','Z','X','C','V','B',
    'N','M',',','.','/',0,'*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,
    '7','8','9','-','4','5','6','+','1','2','3','0','.',0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0
};


struct BufferPool keyboard_buffer;
bool cap_lock;
bool num_lock;
bool scr_lock;

static void keyboard_status_init();
static inline void wait_keyboard_ready();
extern struct BufferPool common_buffer;

void keyboard_init() {
    keyboard_status_init();
}

void int21_keyboard() {
    _out8(PIC0_OCW2,0x61);
    uint8_t data = _in8(PORT_KEYBOARD);
    if(data >= 0x80)
        return;
    else {
        if(!fifo_full(&common_buffer)) {
            if(keyboard_map[data] != 0)
                fifo_put(&common_buffer,keyboard_map[data]);
            else
                fifo_put(&common_buffer,data);
        }
    }
    return;
}

static void keyboard_status_init(){
    wait_keyboard_ready();
    _out8(KEYDATA,0xed);
    wait_keyboard_ready();
    _out8(KEYDATA,0x02);
    cap_lock = false;
    num_lock = true;
    scr_lock = false;
    return;
}

static inline void wait_keyboard_ready(){
    while(1){
        if((_in8(KEYSTAT) & KEYBOARD_NOT_READY) == 0)
            break;
    }
    return;
}
