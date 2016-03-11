#include <kernel.h>

extern struct BufferPool keyboard_buffer;

void KaiOS_main() {
    tty_init();
    gdt_init();
    idt_init();
    pic_init();
    keyboard_init();
    pic_set_mask(PIC0,0xf9);

    while(1) {
        if(keyboard_buffer.front == keyboard_buffer.tail) {
            _hlt();
        } else
            while(keyboard_buffer.front != keyboard_buffer.tail) {
                putchar(keyboard_buffer.buffer[keyboard_buffer.front++]);
                keyboard_buffer.front %= BUFFER_LEN;
            }
    }

    //while(1);
}







