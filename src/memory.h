#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <stdint.h>
#define MEM_TEST_START  0x00400000
#define MEM_TEST_END  0xffffffff
#define DISABLE_CACHE_MASK  0x60000000
#define PAGE_SIZE  4096
#define PAGE_ENTRY_NUM  1024
#define MEM_ITEM_MAX 1024
struct Mem {
    uint32_t head, end;
};

struct MemAdmin {
    uint32_t mem_free_all, lists_len;
    struct Mem mem_lists[MEM_ITEM_MAX];
};
void mem_init();
uint32_t free_mem();
uint32_t** get_kernel_pagedir();
void memset(void* addr,uint8_t value,uint32_t size);
void memcpy(void* to,void* from,uint32_t size);
void kernel_free(void* addr, uint32_t size);
void* kernel_alloc(uint32_t size) ;
#endif // MEMORY_H_INCLUDED
