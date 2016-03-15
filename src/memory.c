#include <memory.h>
#include <stddef.h>
#include <asmfunc.h>
#include <tty.h>
const uint32_t MEM_FREE_HEAD = 0x00400000;
const uint32_t MEM_TEST_START = 0x00400000;
const uint32_t MEM_TEST_END = 0xffffffff;
const uint32_t DISABLE_CACHE_MASK = 0x60000000;
const uint32_t PAGE_SIZE = 4096;

static uint32_t memtest();


struct Bitmap mem_admin;
uint32_t* page_directory;

void mem_init() {
    memset(mem_admin.space, 0, sizeof(mem_admin.space));
    mem_admin.max_bit = (memtest() - MEM_TEST_START) / PAGE_SIZE;
    page_directory = (uint32_t*) palloc();
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

void* palloc() {
    uint32_t address = 0;
    for(uint32_t i = 0; i < MAX_PAGE_NUM_32; i++) {
        if((mem_admin.space[i] & 0xffffffff) != 0xffffffff) {
            for(uint32_t j = 0; j < 32; j++) {
                if((mem_admin.space[i] & (1 << j)) == 0) {
                    address = j * PAGE_SIZE + i * 32 * PAGE_SIZE + MEM_FREE_HEAD;
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
    uint32_t i = (address - MEM_FREE_HEAD) / PAGE_SIZE / 32;
    uint32_t j = (address - MEM_FREE_HEAD) / PAGE_SIZE % 32;
    mem_admin.space[i] ^= (1 << j);
    return;
}

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
        if(*cur != FLAG1){
            *cur = old;
            break;
        }

        *cur ^= 0xffffffff;
        if(*cur != FLAG0){
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
