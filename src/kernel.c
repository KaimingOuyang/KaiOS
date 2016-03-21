#include <kernel.h>

void KaiOS_main() {
    gdt_init();
    mem_init();
    task_admin_init();
    idt_init();
    pic_init();
    keyboard_init();
    pic_set_mask(PIC0, 0xf8);

    task_begin();
    return;
}





