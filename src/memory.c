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
