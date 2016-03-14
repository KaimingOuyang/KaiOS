#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include <stdint.h>

void keyboard_init();
void keyboard_parser(uint16_t data);

#endif // KEYBOARD_H_INCLUDED
