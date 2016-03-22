#include <fifo.h>

void fifo_init(struct BufferPool* fifo) {
    fifo->front = 0;
    fifo->tail = 0;
    return;
}

void fifo_put(struct BufferPool* fifo,uint16_t data) {
    if((fifo->tail+1)%BUFFER_LEN != fifo->front) {
        fifo->buffer[fifo->tail++] = data;
        fifo->tail %= BUFFER_LEN;
    }
    return;
}

uint8_t fifo_get(struct BufferPool* fifo){
    uint8_t data = 0;
    if(fifo->tail != fifo->front){
        data = fifo->buffer[fifo->front++];
        fifo->front %= BUFFER_LEN;
    }
    return data;
}

bool fifo_empty(struct BufferPool* fifo){
    if(fifo->tail == fifo->front)
        return true;
    return false;
}

bool fifo_full(struct BufferPool* fifo) {
    if((fifo->tail+1)%BUFFER_LEN == fifo->front)
        return true;
    return false;
}

void fifo_putback(struct BufferPool* fifo,uint16_t data){
    fifo->front = (fifo->front-1+BUFFER_LEN) % BUFFER_LEN;
    fifo->buffer[fifo->front] = data;
    return;
}
