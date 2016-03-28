#include <memory.h>
#include <stddef.h>
#include <asmfunc.h>

struct MemAdmin mem_admin;
uint32_t** kernel_page_directory;

static uint32_t memtest();

void mem_init() {
    uint32_t mem_free_head;
    uint32_t mem_free_end;
    mem_free_head = MEM_TEST_START;
    mem_free_end = memtest();

    mem_admin.lists_len = 1;
    mem_admin.mem_free_all = mem_free_end - mem_free_head;
    mem_admin.mem_lists[0].head = mem_free_head;
    mem_admin.mem_lists[0].end = mem_free_end;

    kernel_page_directory = (uint32_t**) kernel_alloc(PAGE_SIZE);

    for(uint32_t index_1 = 0; index_1 < PAGE_ENTRY_NUM; index_1++) {
        kernel_page_directory[index_1] = (uint32_t*) kernel_alloc(PAGE_SIZE);

        for(uint32_t index_2 = 0; index_2 < PAGE_ENTRY_NUM; index_2++)
            kernel_page_directory[index_1][index_2] = (index_1 * PAGE_ENTRY_NUM + index_2) * PAGE_SIZE | 3;

        kernel_page_directory[index_1] = (uint32_t*)((uint32_t)kernel_page_directory[index_1] | 3);
    }

    _set_page_directory(kernel_page_directory);
    _enable_paging();
    return;
}

uint32_t free_mem(){
    return mem_admin.mem_free_all;
}

// physical address
void* kernel_alloc(uint32_t size_tmp) {
    uint32_t address = 0;
    uint32_t size = size_tmp / PAGE_SIZE * PAGE_SIZE + (size_tmp % PAGE_SIZE == 0 ? 0 : PAGE_SIZE);

    for(uint32_t index_1 = 0; index_1 < mem_admin.lists_len; index_1++) {
        struct Mem* cur = &mem_admin.mem_lists[index_1];

        if(cur->end - cur->head >= size) {
            address = cur->head;
            cur->head += size;
            mem_admin.mem_free_all -= size;

            if(cur->head == cur->end) {
                for(uint32_t index_2 = index_1 + 1; index_2 < mem_admin.lists_len; index_2++)
                    mem_admin.mem_lists[index_2 - 1] = mem_admin.mem_lists[index_2];

                mem_admin.lists_len--;
            }
        }
    }

    return (void*) address;
}
/*
void* malloc(uint32_t size_tmp){

}


API
// virtual address
void* user_alloc(uint32_t size, enum AllocType type, uint32_t task_num) {

}
void free();
*/

void kernel_free(void* paddr, uint32_t size_tmp) {
    uint32_t addr = (uint32_t) paddr;
    uint32_t index_1 = 0;
    uint32_t size = size_tmp / PAGE_SIZE * PAGE_SIZE + (size_tmp % PAGE_SIZE == 0 ? 0 : PAGE_SIZE);

    for(index_1 = 0; index_1 < mem_admin.lists_len; index_1++) {
        struct Mem* cur = &mem_admin.mem_lists[index_1];

        if(addr < cur->head) {
            for(uint32_t index_2 = mem_admin.lists_len - 1; index_2 >= index_1; index_2--)
                mem_admin.mem_lists[index_2 + 1] = mem_admin.mem_lists[index_2];

            break;
        }
    }

    mem_admin.mem_lists[index_1].head = addr;
    mem_admin.mem_lists[index_1].end = addr + size;
    mem_admin.lists_len++;
    uint8_t v[mem_admin.lists_len];
    memset(v, 0, sizeof(v));

    for(index_1 = 0; index_1 < mem_admin.lists_len - 1; index_1++) {
        struct Mem* cur = &mem_admin.mem_lists[index_1];
        struct Mem* next = &mem_admin.mem_lists[index_1 + 1];

        if(cur->end == next->head) {
            next->head = cur->head;
            v[index_1] = 1;
        }
    }

    uint32_t new_len = 0;

    for(index_1 = 0; index_1 < mem_admin.lists_len; index_1++) {
        if(v[index_1] == 0)
            mem_admin.mem_lists[new_len++] = mem_admin.mem_lists[index_1];
    }

    mem_admin.mem_free_all += size;
    mem_admin.lists_len = new_len;
    return;
}


void memset(void* addr, uint8_t value, uint32_t size) {
    if(addr == NULL)
        return;

    uint32_t fast_index = size / 8;
    uint32_t slow_index = size % 8;

    uint64_t* addr_64 = (uint64_t*) addr;
    uint8_t* addr_8;

    uint32_t cnt = 0;
    uint64_t value_64 = 0;

    for(uint32_t index = 0; index < 8; index++)
        value_64 |= (value << (index * 8));

    while(cnt < fast_index) {
        addr_64[0] = value_64;
        addr_64++;
        cnt++;
    }

    cnt = 0;
    addr_8 = (uint8_t*) addr_64;

    while(cnt < slow_index) {
        addr_8[0] = value;
        addr_8++;
        cnt++;
    }

    return;
}

void memcpy(void* to, void* from, uint32_t size) {
    if(to == NULL || from == NULL)
        return;

    uint32_t fast_index = size / 8;
    uint32_t slow_index = size % 8;

    uint64_t* to_64 = to;
    uint64_t* from_64 = from;
    uint8_t* to_8;
    uint8_t* from_8;

    uint32_t index = 0;

    while(index < fast_index) {
        to_64[0] = from_64[0];
        to_64++;
        from_64++;
        index++;
    }

    index = 0;
    to_8 = (uint8_t*) to_64;
    from_8 = (uint8_t*) from_64;

    while(index < slow_index) {
        to_8[0] = from_8[0];
        to_8++;
        from_8++;
        index++;
    }

    return;
}
/*
void* kernel_malloc(uint32_t size){
    uint32_t page_len = size / PAGE_SIZE + (size % PAGE_SIZE == 0 ? 0 : 1);
    uint32_t
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
