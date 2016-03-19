#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED
#include <stdint.h>
#include <fifo.h>
#define MAX_TASK 128
#define VGAWIDTH 80
#define VGAHEIGHT 25
struct Tss32 {
    uint32_t backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
    uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldtr, iomap;
};

struct Task {
    uint16_t selector, task_id;
    uint16_t* tty_buffer;
    uint32_t tty_pos_cur,tty_pos_start,tty_pos_end;
    struct Tss32 tss;
    struct BufferPool fifo;
};

struct TaskAdmin {
    uint32_t task_len, show_screen_id;
    uint32_t ready_cur, ready_len;
    uint32_t sleep_len;
    struct Task tasks[MAX_TASK];
    struct Task* running;
    struct Task* ready[MAX_TASK];
    struct Task* sleep[MAX_TASK];
};
void task_init(struct Task* task,uint32_t selector,uint32_t id,
               uint32_t data_sel,uint32_t code_sel,uint32_t esp,uint32_t eip, uint32_t page_dir);
void task_admin_init();
#endif // TASK_H_INCLUDED
