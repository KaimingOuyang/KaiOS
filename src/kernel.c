#include <kernel.h>

void KaiOS_main() {
    tty_init();
    gdt_init();
    idt_pic_init();
    printf("Hello World\n");
    printf("Kaiming\n");
    while(1);
}







