#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include <stdint.h>
#include <fifo.h>
void keyboard_init();
void keyboard_parser(uint16_t data, struct BufferPool* common_buffer);

#endif // KEYBOARD_H_INCLUDED
