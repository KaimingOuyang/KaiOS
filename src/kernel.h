#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED
#include <io.h>
#include <tty.h>
#include <fifo.h>
#include <stddef.h>
#include <stdbool.h>
#include <gdt_idt.h>
#include <memory.h>
#include <asmfunc.h>
#include <keyboard.h>
#include <interrupts.h>
#include <task.h>
#if !defined(__i686__)
#error "This kernel needs to be compiled with a i686-elf compiler"
#endif

#endif // KERNEL_H_INCLUDED
