#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#include <stdint.h>

void io_init();
void printf(const char* str, ...);
void putchar(const char c);
uint8_t* floppy_read(uint32_t addr, uint8_t size);

#endif // IO_H_INCLUDED
