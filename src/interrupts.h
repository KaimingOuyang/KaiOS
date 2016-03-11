#ifndef INTERRUPTS_H_INCLUDED
#define INTERRUPTS_H_INCLUDED

#include <stdint.h>
#define PIC0 0
#define PIC1 1
#define BUFFER_LEN 256

struct BufferPool{
    uint8_t buffer[BUFFER_LEN];
    volatile uint16_t front,tail;
};

void pic_init();
void keyboard_init();
void int21_keyboard();
void pic_set_mask(uint8_t pic,uint16_t attr);

#endif // INTERRUPTS_H_INCLUDED
