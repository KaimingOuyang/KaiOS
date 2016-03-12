#include <memory.h>
#include <stddef.h>
// whether the memset function has some bugs or efficiency problems?
void memset(void* addr,uint8_t value,uint32_t size){
    if(addr == NULL)
        return;
    uint8_t* addrtmp = (uint8_t*) addr;
    uint32_t cnt = 0;
    while(cnt < size){
        addrtmp[0] = value;
        addrtmp++;
        cnt++;
    }
    return;
}

void memcpy(void* to,void* from,uint32_t size){
    if(to == NULL || from == NULL)
        return;
    uint8_t* to_tmp = to;
    uint8_t* from_tmp = from;
    uint32_t index = 0;
    while(size > index){
        to_tmp[index] = from_tmp[index];
        index++;
    }
    return;
}
