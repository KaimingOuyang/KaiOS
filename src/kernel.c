#include <kernel.h>

void milk_main() {
    gdt_init();
    mem_init();
    idt_init();
    pic_init();
    keyboard_init();
    pic_set_mask(PIC0, 0xb8);
    //show_logo();
    task_admin_init();
    io_init();
    task_begin();
    return;
}




