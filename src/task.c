#include <task.h>
#include <asmfunc.h>
#include <gdt_idt.h>
#include <memory.h>
#include <tty.h>
struct TaskAdmin* task_admin;
extern struct GdtDescriptor* GDT;

void task_admin_init() {
    task_admin = (struct TaskAdmin*) kernel_alloc(sizeof(struct TaskAdmin));
    task_init(&task_admin->tasks[1], 1 * 8, 1, 0, 0, 0, 0, 0);
    task_admin->show_screen_id = 1;
    task_admin->task_len = 1;
    task_admin->running = &task_admin->tasks[1];
    task_admin->ready_cur = 0;
    task_admin->ready_len = 0;
    task_admin->sleep_len = 0;
    set_gdt_struct(GDT + 1, 103, (uint32_t)&task_admin->tasks[1].tss, TSS_AR);
    _load_tr(1 * 8);
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
    task[1].esp = (ui.nt32_t)kernel_alloc(64 * (1 << 10)) + 64 * (1 << 10);
    task[1].ebp = 0;
    task[1].esi = 0;
    task[1].edi = 0;
    task[1].es = 2001 * 8;
    task[1].cs = 1001 * 8;
    task[1].ss = 2001 * 8;
    task[1].ds = 2001 * 8;
    task[1].fs = 2001 * 8;
    task[1].gs = 2001 * 8;
    task[1].cr3 = _load_page_directory();
    */

    //printf("eip:%X,esp:%X", task[1].eip, task[1].esp);
//    _load_tr(1 * 8);
//    _switch_task_b();
    //task_init();
    return;
}

void task_init(struct Task* task, uint32_t selector, uint32_t id,
               uint32_t data_sel, uint32_t code_sel, uint32_t esp, uint32_t eip, uint32_t page_dir) {
    fifo_init(&task->fifo);
    task->selector = selector;
    task->task_id = id;
    task->tty_buffer = (uint16_t*) kernel_alloc(sizeof(uint16_t) * VGAHEIGHT * VGAWIDTH);
    tty_buffer_init(task);
    task->tty_pos_cur = 0;
    task->tty_pos_start = 0;
    task->tty_pos_end = 0;
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
    return;
}
