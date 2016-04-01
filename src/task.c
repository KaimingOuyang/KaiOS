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

static void soft_task_switch();

void task_admin_init() {

    task_admin = (struct TaskAdmin*) kernel_alloc(sizeof(struct TaskAdmin));
    memset(task_admin->visit, false, sizeof(task_admin->visit));
    task_init(&task_admin->tasks[1], 1 * 8, 1, (uint32_t)kernel_alloc(sizeof(uint16_t) * VGAHEIGHT * VGAWIDTH),
              0, 0, 0, (uint32_t)&task_begin, kernel_page_directory);
    task_admin->show_id = 1;
    task_admin->running = &task_admin->tasks[1];
    task_admin->ready_head = NULL;
    task_admin->ready_end = NULL;
    task_admin->sleep_head = NULL;
    task_admin->sleep_end = NULL;
    _load_tr(1 * 8);

    task_admin->tasks[0].tty_buffer = (uint16_t*) VGAHEAD;
    tty_init(&task_admin->tasks[0]);
    make_title(MACHINE_TITLE, &task_admin->tasks[0], 0);
    update_cursor();
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
    make_title(MACHINE_TITLE, task, 0);
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
    set_gdt_struct(id, 103, (uint32_t)&task_admin->tasks[id].tss, TSS_AR);
    return;
}

void task_begin() {
    uint8_t data = 0;

    while(1) {
        if(fifo_empty(&task_admin->running->fifo)) {
            soft_task_switch();
        } else {
            data = fifo_get(&task_admin->running->fifo);

            if(data < 0xff) // range for keyboard
                keyboard_parser(data, &task_admin->running->fifo);
        }
    }
}

static void soft_task_switch(){
    _cli();
    if(task_admin->ready_head != NULL) {
        task_admin->ready_end->next_ready = task_admin->running;
        task_admin->ready_end = task_admin->running;
        task_admin->running = task_admin->ready_head;
        task_admin->ready_head = task_admin->ready_head->next_ready;
        _switch_task(0, task_admin->running->selector);
    }
    _sti();
    return;
}
