#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED

#include <tty.h>
#include <fifo.h>
#include <stdbool.h>
#include <gdt_idt.h>
#include <memory.h>
#include <asmfunc.h>
#include <keyboard.h>
#include <interrupts.h>

#if !defined(__i686__)
#error "This kernel needs to be compiled with a i686-elf compiler"
#endif

extern bool cap_lock;
extern bool num_lock;
extern bool scr_lock;

#endif // KERNEL_H_INCLUDED
