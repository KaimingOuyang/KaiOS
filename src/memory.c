#include <memory.h>
#include <stddef.h>
#include <asmfunc.h>
#include <tty.h>

const uint32_t MEM_TEST_START = 0x00400000;
const uint32_t MEM_TEST_END = 0xffffffff;
const uint32_t DISABLE_CACHE_MASK = 0x60000000;
const uint32_t PAGE_SIZE = 4096;
const uint32_t PAGE_ENTRY_NUM = 1024;
static uint32_t memtest();

enum AllocType{
    CODE,STACK,PILE
};

struct PageAllocStatus {
    uint32_t code_trace, stack_trace, pile_trace;
};

struct TaskPageDirector {
    uint32_t task_len;
    uint32_t* page_directorys[MAX_TASK_NUM];
    struct PageAllocStatus status[MAX_TASK_NUM];
};

uint32_t mem_free_head;
uint32_t mem_free_end;

struct Bitmap mem_admin;
struct TaskPageDirector page_admin;

uint32_t* kernel_page_directory;
uint32_t* kernel_page_table;

void mem_init() {
    mem_free_end = memtest();
    memset(mem_admin.space, 0, sizeof(mem_admin.space));

    kernel_page_directory = (uint32_t*) MEM_TEST_START;
    uint32_t page_len = mem_free_end / PAGE_SIZE / PAGE_ENTRY_NUM + (mem_free_end / PAGE_SIZE % PAGE_ENTRY_NUM == 0 ? 0 : 1);
    for(uint32_t index_1 = 0; index_1 < page_len; index_1++) {
        kernel_page_table = (uint32_t*) (MEM_TEST_START + (index_1 + 1) * PAGE_SIZE);

        for(uint32_t index_2 = 0; index_2 < PAGE_ENTRY_NUM; index_2++)
            kernel_page_table[index_2] = ((index_1 * PAGE_ENTRY_NUM + index_2) * PAGE_SIZE) | 3;
        kernel_page_directory[index_1] = (uint32_t) kernel_page_table | 3;
    }

    mem_free_head = MEM_TEST_START + (page_len + 1) * PAGE_SIZE;
    mem_admin.max_bit = (mem_free_end - mem_free_head) / PAGE_SIZE;

    for(uint32_t index = 1; index < MAX_TASK_NUM; index++) {
        page_admin.status[index].code_trace = 0; // 0 default
        page_admin.status[index].stack_trace = 2 * (1 << 30); // 2G default
        page_admin.status[index].pile_trace = 2 * (1 << 30); // 2G default
        page_admin.page_directorys[index] = NULL;
        page_admin.task_len = 2;
    }
    page_admin.page_directorys[1] = kernel_page_directory;

    _set_page_directory(kernel_page_directory);
    _enable_paging();
    return;
}

// whether the memset function has some bugs or efficiency problems?
void memset(void* addr, uint8_t value, uint32_t size) {
    if(addr == NULL)
        return;
    uint8_t* addrtmp = (uint8_t*) addr;
    uint32_t cnt = 0;
    while(cnt < size) {
        addrtmp[0] = value;
        addrtmp++;
        cnt++;
    }
    return;
}

void memcpy(void* to, void* from, uint32_t size) {
    if(to == NULL || from == NULL)
        return;
    uint8_t* to_tmp = to;
    uint8_t* from_tmp = from;
    uint32_t index = 0;
    while(size > index) {
        to_tmp[index] = from_tmp[index];
        index++;
    }
    return;
}
/*
void* palloc(uint32_t size,enum AllocType type) {
    uint32_t address = 0;
    for(uint32_t i = 0; i < MAX_PAGE_NUM_32; i++) {
        if((mem_admin.space[i] & 0xffffffff) != 0xffffffff) {
            for(uint32_t j = 0; j < 32; j++) {
                if((mem_admin.space[i] & (1 << j)) == 0) {
                    address = j * PAGE_SIZE + i * 32 * PAGE_SIZE + mem_free_head;
                    mem_admin.space[i] |= (1 << j);
                    break;
                }
            }
        }
        if(address != 0)
            break;
    }
    return (void*) address;
}

void free(void* addr) {
    uint32_t address = (uint32_t) addr;
    uint32_t i = (address - mem_free_head) / PAGE_SIZE / 32;
    uint32_t j = (address - mem_free_head) / PAGE_SIZE % 32;
    mem_admin.space[i] ^= (1 << j);
    return;
}
*/
static uint32_t memtest() {
    uint32_t cr0 = _load_cr0();
    cr0 |= DISABLE_CACHE_MASK;
    _set_cr0(cr0);

    const uint32_t FLAG0 = 0xaaaaaaaa, FLAG1 = 0x55555555;
    volatile uint32_t* cur;
    uint32_t old;
    uint32_t i;

    for(i = MEM_TEST_START; i <= MEM_TEST_END; i += 0x1000) {
        cur = (uint32_t*) i;
        old = *cur;
        *cur = FLAG0;
        *cur ^= 0xffffffff;
        if(*cur != FLAG1) {
            *cur = old;
            break;
        }

        *cur ^= 0xffffffff;
        if(*cur != FLAG0) {
            *cur = old;
            break;
        }
        *cur = old;
    }
    cr0 = _load_cr0();
    cr0 &= ~DISABLE_CACHE_MASK;
    _set_cr0(cr0);
    return i;
}
