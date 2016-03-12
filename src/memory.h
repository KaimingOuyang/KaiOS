#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <stdint.h>
void memset(void* addr,uint8_t value,uint32_t size);
void memcpy(void* to,void* from,uint32_t size);
#endif // MEMORY_H_INCLUDED
