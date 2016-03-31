#include <shell.h>
#include <string.h>
#include <tty.h>
#include <io.h>
#include <gdt_idt.h>
#include <memory.h>

#define FAT_ADDR 0x200
#define ROOT_DIRECTORY 0x2600
#define DATA_ADDR 0x4200
struct DirectoryEntry {
    char name[11];
    uint8_t attr;
    uint8_t res[10];
    uint16_t wtime;
    uint16_t wdate;
    uint16_t hclus;
    uint32_t fsize;
};

struct ELFHead{
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

struct ELFProHead{
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
};

extern struct TaskAdmin* task_admin;
uint32_t code_addr;
uint32_t data_addr;
void command_line_parser(const char* command) {
    if(streq(command, "clear")) {
        tty_init(&task_admin->tasks[0]);
    } else if(streq(command, "free")) {
        printf("\nMem Free:%uM\n", free_mem() / 1024 / 1024);
    } else if(streq(command, "ls")) {
        printf("\n\tName\tType\tDate\t\tSize\n");
        struct DirectoryEntry* en = floppy_read(ROOT_DIRECTORY, 14);

        while(1) {
            if(en->name[0] == 0)
                break;

            char name[9], type[4];
            uint32_t mon = en->wtime >> 8;
            uint32_t day = en->wtime & 0xff;
            memcpy(name, en->name, 8);
            memcpy(type, en->name + 8, 3);
            name[8] = '\0';
            type[3] = '\0';
            printf("%s\t%s\t%u-%u-%u\t%u\n", name, type, en->wdate, mon, day, en->fsize);
            en++;
        }

        printf("\n");
    } else if(streqn(command, "./", 2) && strlen(command + 2) > 0) {
        struct DirectoryEntry* en = floppy_read(ROOT_DIRECTORY, 14);

        while(1) {
            if(en->name[0] == 0) {
                printf("\n%s:File not found.", command + 2);
                break;
            }

            char name[9], type[4];
            uint32_t index = 0;

            for(; index < 8; index++)
                if(en->name[index] != ' ')
                    break;
            memcpy(name, en->name + index, 8 - index);
            name[8-index] = '\0';
            index = 0;
            for(; index < 3; index++)
                if(en->name[8+index] != ' ')
                    break;
            memcpy(type, en->name + 8 + index, 3 - index);
            type[3-index] = '\0';

            if(streq(name, command + 2)) {
                if(streq(type, "mk")) {
                    // load application
                    uint32_t app_addr = DATA_ADDR + 512 * en->hclus;
                    struct ELFHead* elf_head = floppy_read(app_addr,en->fsize / 512 + 1);
                    struct ELFProHead* elf_phead = elf_head + 1;
                    uint8_t* p = (uint8_t) elf_head;

                    code_addr = (uint32_t) kernel_alloc(4*1024*1024);
                    data_addr = (uint32_t) kernel_alloc(4*1024*1024);
                    //set_gdt_struct(1002,4*1024*1024,code_addr,OS_GDT_CODE_AR + 0x60);
                    set_gdt_struct(1002,4*1024*1024-1,code_addr,OS_GDT_CODE_AR);
                    //set_gdt_struct(2002,4*1024*1024,data_addr,OS_GDT_DATA_AR + 0x60);
                    set_gdt_struct(2002,4*1024*1024-1,data_addr,OS_GDT_DATA_AR);
                    uint8_t* code_si = (uint8_t*) (elf_phead->p_offset + 0x1000);
                    uint8_t* code_de = (uint8_t*) code_addr + elf_phead->p_vaddr;

                    for(uint32_t index_1=0;index_1<elf_phead->p_filesz;index_1++)
                        code_de[index_1] = code_si[index_1];

                    elf_phead++;
                    uint8_t* data_si = (uint8_t*) (elf_phead->p_offset + 0x1000);
                    uint8_t* data_de = (uint8_t*) (data_addr + elf_phead->p_vaddr);
                    for(uint32_t index_1=0;index_1<elf_phead->p_filesz;index_1++)
                        data_de[index_1] = data_si[index_1];

                    printf("\n");

                    _start_app(elf_head->e_entry,1002*8,0x1000,
                              2002*8,&task_admin->running->tss.esp0);
                    kernel_free(code_addr,4*1024*1024);
                    kernel_free(data_addr,4*1024*1024);

                } else
                    printf("\n%s:File type error.", name);

                break;
            }
            en++;
        }

        printf("\n");
    } else {
        // add command for shell
        printf("\n%s: command not found\n", command);
    }

    make_title(MACHINE_TITLE, &task_admin->tasks[0], 0);

    return;
}
