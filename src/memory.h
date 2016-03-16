#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <stdint.h>
#define MAX_PAGE_NUM_32 32768
#define MAX_TASK_NUM 1001

enum AllocType {
    CODE = 0, STACK = 1, PILE = 2
};

void mem_init();
void memset(void* addr,uint8_t value,uint32_t size);
void memcpy(void* to,void* from,uint32_t size);
void* page_alloc();
void free(void* vaddr, uint32_t size, uint32_t task_num);
void* malloc(uint32_t size, enum AllocType type, uint32_t task_num);

#endif // MEMORY_H_INCLUDED
