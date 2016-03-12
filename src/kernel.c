#include <kernel.h>

void KaiOS_main() {
    tty_init();
    gdt_init();
    idt_init();
    pic_init();
    keyboard_init();
    fifo_init(&common_buffer);
    pic_set_mask(PIC0,0xf9);

    uint16_t data = 0;
    while(1) {
        if(fifo_empty(&common_buffer)) {
            _hlt();
        } else{
            data = fifo_get(&common_buffer);
            putchar(data);
        }
        /*
            while(keyboard_buffer.front != keyboard_buffer.tail) {
                putchar(keyboard_buffer.buffer[keyboard_buffer.front++]);
                keyboard_buffer.front %= BUFFER_LEN;
            }
        */
    }
    //while(1);
}







