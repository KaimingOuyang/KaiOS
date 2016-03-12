#ifndef FIFO_H_INCLUDED
#define FIFO_H_INCLUDED

#define BUFFER_LEN 512

#include <stdint.h>
#include <stdbool.h>

struct BufferPool{
    uint16_t buffer[BUFFER_LEN];
    uint16_t front,tail;
};

void fifo_init(struct BufferPool* fifo);
void fifo_put(struct BufferPool* fifo,uint16_t data);
uint8_t fifo_get(struct BufferPool* fifo);
bool fifo_empty(struct BufferPool* fifo);
bool fifo_full(struct BufferPool* fifo);
void fifo_putback(struct BufferPool* fifo,uint16_t data);

#endif // FIFO_H_INCLUDED
