#include <kernel.h>

static void task_begin();
extern struct TaskAdmin* task_admin;
void KaiOS_main() {
    gdt_init();
    mem_init();
    task_admin_init();
    tty_init();
    idt_init();
    pic_init();
    keyboard_init();
    pic_set_mask(PIC0, 0xf8);

    task_begin();
    return;
}

static void task_begin() {
    uint8_t data = 0;

    while(1) {
        if(fifo_empty(&task_admin->running->fifo)) {
            _hlt();
        } else {
            data = fifo_get(&task_admin->running->fifo);

            if(data < 0xff) // range for keyboard
                keyboard_parser(data, &task_admin->running->fifo);
        }
    }
}



