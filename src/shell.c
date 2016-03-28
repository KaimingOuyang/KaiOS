#include <shell.h>
#include <string.h>
#include <tty.h>
#include <io.h>
#include <memory.h>

#define FAT_ADDR 0x200
#define ROOT_DIRECTORY 0x2600
#define DATA_ADDR 0x4200


extern struct TaskAdmin* task_admin;

void command_line_parser(const char* command) {
    if(streq(command, "clear")) {
        tty_init(&task_admin->tasks[0]);
    } else if(streq(command, "free")) {
        printf("\nMem Free:%uM\n", free_mem() / 1024 / 1024);
    } else if(streq(command,"ls")){

    } else if(streq(command,"")){
    }
    else {
        // add command for shell
        printf("\n%s: command not found\n", command);
    }

    make_title(MACHINE_TITLE, &task_admin->tasks[0], 0);

    return;
}
