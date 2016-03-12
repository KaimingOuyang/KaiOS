#ifndef INTERRUPTS_H_INCLUDED
#define INTERRUPTS_H_INCLUDED

#include <stdint.h>

#define PIC0 0
#define PIC1 1

void pic_init();
void keyboard_init();
void int21_keyboard();
void pic_set_mask(uint8_t pic,uint16_t attr);

#endif // INTERRUPTS_H_INCLUDED
