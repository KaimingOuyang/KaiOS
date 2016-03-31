#include <io.h>
#include <asmfunc.h>
#include <task.h>
#include <tty.h>
#include <memory.h>
//ebx is chosen for recognizing which function to call
//eax is chosen for parameter
extern struct TaskAdmin* task_admin;
extern uint32_t code_addr;
extern uint32_t data_addr;
void int40_api(int32_t edi, int32_t esi, int32_t ebp, int32_t esp,
               int32_t ebx, int32_t edx, int32_t ecx, int32_t eax) {
    if(ebx == 1) {

    } else if(ebx == 2) {
        char ch = eax & 0xff;
        putchar(ch);
    } else if(ebx == 3) {
        _end_app();
        make_title(MACHINE_TITLE, &task_admin->tasks[0], 0);
        update_cursor();
        kernel_free(code_addr,4*1024*1024);
        kernel_free(data_addr,4*1024*1024);

        task_begin();
    }

    return;
}
