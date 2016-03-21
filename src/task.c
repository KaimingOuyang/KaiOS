#include <task.h>
#include <asmfunc.h>
#include <gdt_idt.h>
#include <memory.h>
#include <tty.h>
#include <fifo.h>
#include <keyboard.h>
#include <stddef.h>
struct TaskAdmin* task_admin;
extern struct GdtDescriptor* GDT;
extern uint32_t** kernel_page_directory;
void task_tmp(){
    printf("Hello Task world");
    while(1);
}

void task_admin_init() {

    task_admin = (struct TaskAdmin*) kernel_alloc(sizeof(struct TaskAdmin));
    memset(task_admin->visit, false, sizeof(task_admin->visit));
    task_init(&task_admin->tasks[1], 1 * 8, 1, (uint32_t)kernel_alloc(sizeof(uint16_t) * VGAHEIGHT * VGAWIDTH),
               0, 0, 0, (uint32_t)&task_begin, kernel_page_directory);
    task_init(&task_admin->tasks[0], 0, 0, (uint32_t)VGAHEAD, 0, 0, 0, 0, 0);
    task_admin->show_id = 1;
    task_admin->running = &task_admin->tasks[1];
    task_admin->ready_head = NULL;
    task_admin->ready_end = NULL;
    task_admin->sleep_head = NULL;
    task_admin->sleep_end = NULL;
    set_gdt_struct(GDT + 1, 103, (uint32_t)&task_admin->tasks[1].tss, TSS_AR);
    _load_tr(1 * 8);

    tty_init(&task_admin->tasks[0]);
    /*
    task_init(&task_admin->tasks[2], 2 * 8, 2, kernel_alloc(sizeof(uint16_t) * VGAHEIGHT * VGAWIDTH),
                  KERNEL_DATA_SEGMENT * 8, KERNEL_CODE_SEGMENT * 8, kernel_alloc(64 * (1 << 10)) + 64 * (1 << 10),
                  (uint32_t)&task_begin, (uint32_t)kernel_page_directory);
    set_gdt_struct(GDT + 2, 103, (uint32_t)&task_admin->tasks[2].tss, TSS_AR);
    _switch_task(0,0);
    */
//    uint8_t* iomap = (uint8_t*)kernel_alloc(PAGE_SIZE * 2); // 8KB
//    memset(iomap, 0, PAGE_SIZE * 2);
//    struct Tss32 task[2];
//    task[0].ldtr = 0;
//    task[0].iomap = 0x40000000;
//    set_gdt_struct(GDT + ,)
//    _load_tr();
/*
    task[1].ldtr = 0;
    task[1].iomap = 0x40000000;
    task[1].eip = (uint32_t)&tmp_task;
    task[1].eflags = 0x00000202;
    task[1].eax = 0;
    task[1].ecx = 0;
    task[1].edx = 0;
    task[1].ebx = 0;
    task[1].esp = (uint32_t)kernel_alloc(64 * (1 << 10)) + 64 * (1 << 10);
    task[1].ebp = 0;
    task[1].esi = 0;
    task[1].edi = 0;
    task[1].es = 2001 * 8;
    task[1].cs = 1001 * 8;
    task[1].ss = 2001 * 8;
    task[1].ds = 2001 * 8;
    task[1].fs = 2001 * 8;
    task[1].gs = 2001 * 8;
    task[1].cr3 = (uint32_t)kernel_page_directory;
*/

    //printf("eip:%X,esp:%X", task[1].eip, task[1].esp);
//    _load_tr(1 * 8);
//    _switch_task_b();
    //task_init();
    return;
}

void task_init(struct Task* task, uint16_t selector, uint32_t id, uint16_t* buffer_addr,
               uint32_t data_sel, uint32_t code_sel, uint32_t esp, uint32_t eip, uint32_t page_dir) {
    task_admin->visit[id] = true;
    fifo_init(&task->fifo);
    task->selector = selector;
    task->task_id = id;
    task->tty_buffer = buffer_addr;
    tty_init(task);
    task->tss.ldtr = 0;
    task->tss.iomap = 0x40000000;
    task->tss.eip = eip;
    task->tss.eflags = 0x00000202;
    task->tss.eax = 0;
    task->tss.ecx = 0;
    task->tss.edx = 0;
    task->tss.ebx = 0;
    task->tss.esp = esp;
    task->tss.ebp = 0;
    task->tss.esi = 0;
    task->tss.edi = 0;
    task->tss.es = data_sel;
    task->tss.cs = code_sel;
    task->tss.ss = data_sel;
    task->tss.ds = data_sel;
    task->tss.fs = data_sel;
    task->tss.gs = data_sel;
    task->tss.cr3 = page_dir;
    task->next_ready = NULL;
    task->next_sleep = NULL;
    return;
}

void task_begin() {
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
