#include <kernel.h>

extern bool cap_lock;
extern bool num_lock;
extern bool scr_lock;
extern struct BufferPool common_buffer;

void KaiOS_main() {
    tty_init();
    gdt_init();
    idt_init();
    pic_init();
    keyboard_init();
    fifo_init(&common_buffer);
    pic_set_mask(PIC0,0xf9);

    uint8_t data = 0;
    while(1) {
        if(fifo_empty(&common_buffer)) {
            _hlt();
        } else {
            data = fifo_get(&common_buffer);
            if(data < 0xff) // range for keyboard
                keyboard_parser(data);
        }
    }
    //while(1);
}







