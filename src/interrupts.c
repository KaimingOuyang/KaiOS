#include <interrupts.h>
#include <tty.h>
#include <asmfunc.h>
#include <stdbool.h>
#include <fifo.h>
#include <task.h>

#define PIC0_ICW1 0x0020
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021

#define PIC1_ICW1 0x00a0
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1

#define PIC0_IMR 0x0021
#define PIC1_IMR 0x00a1

#define PIC0_OCW2 0x0020
#define PIC1_OCW2 0x00a0

#define PORT_KEYBOARD 0x0060

#define PIC_MODE_PORT 0x43
#define RATE_MODE 0x34
#define PIT_CHANNEL_0 0x40

extern struct TaskAdmin* task_admin;
static void set_pit();

void pic_init() {

    _out8(PIC0_IMR, 0xff);
    _out8(PIC1_IMR, 0xff);

    _out8(PIC0_ICW1, 0x11);
    _out8(PIC0_ICW2, 0x20);
    _out8(PIC0_ICW3, 0x04);
    _out8(PIC0_ICW4, 0x01);

    _out8(PIC1_ICW1, 0x11);
    _out8(PIC1_ICW1, 0x28);
    _out8(PIC1_ICW1, 0x04);
    _out8(PIC1_ICW1, 0x01);

    _out8(PIC0_IMR, 0xfb);
    _out8(PIC1_IMR, 0xff);
    _sti();
    set_pit();
    return;
}

// 10ms interrupt
static void set_pit() {
    _out8(PIC_MODE_PORT, RATE_MODE);
    _out8(PIT_CHANNEL_0, 0x9c);
    _out8(PIT_CHANNEL_0, 0x2e);
    return;
}

void pic_set_mask(uint8_t pic, uint16_t attr) {
    switch(pic) {
    case PIC0:
        _out8(PIC0_IMR, attr);
        break;

    case PIC1:
        _out8(PIC1_IMR, attr);
        break;
    }

    return;
}

void int20_timer() {
    _out8(PIC0_OCW2, 0x60);
    if(task_admin->ready_len > 0) {
        uint32_t ready_id = task_admin->ready[task_admin->ready_cur]->task_id;
        struct Task* temp = task_admin->running;
        task_admin->running = &task_admin->tasks[ready_id];
        task_admin->ready[task_admin->ready_cur] = temp;
        task_admin->ready_cur = (task_admin->ready_cur + 1) % task_admin->ready_len;
        _switch_task(0, task_admin->running->selector);
    }

    return;
}

void int21_keyboard() {
    _out8(PIC0_OCW2, 0x61);
    uint8_t data = _in8(PORT_KEYBOARD);

    if(data >= 0x80 && data != 0xaa && data != 0xb6 && data != 0xe0 && data != 0xe1) // escaped and shift character
        return;
    else {
        uint32_t id = task_admin->show_screen_id;

        if(!fifo_full(&task_admin->tasks[id].fifo))
            fifo_put(&task_admin->tasks[id].fifo, data);
    }

    return;
}

