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

extern struct BufferPool common_buffer;

void int21_keyboard() {
    _out8(PIC0_OCW2,0x61);
    uint8_t data = _in8(PORT_KEYBOARD);
    if(data >= 0x80 && data != 0xaa && data != 0xb6 && data != 0xe0 && data != 0xe1) // escaped and shift character
        return;
    else if(!fifo_full(&common_buffer))
        fifo_put(&common_buffer,data);
    return;
}

