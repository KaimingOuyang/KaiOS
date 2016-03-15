#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <stdint.h>
#define MAX_PAGE_NUM_32 32768 // prime number
#define MAX_TASK_NUM 1001
struct Bitmap {
    uint32_t space[MAX_PAGE_NUM_32];
    uint32_t max_bit;
};

void* palloc();
void free(void* addr);
void mem_init();
void memset(void* addr,uint8_t value,uint32_t size);
void memcpy(void* to,void* from,uint32_t size);
#endif // MEMORY_H_INCLUDED
