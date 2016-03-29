#include <io.h>
//ebx is chosen for recognizing which function to call
//eax is chosen for parameter
void int40_api(int32_t edi, int32_t esi, int32_t ebp, int32_t esp,
              int32_t ebx, int32_t edx, int32_t ecx, int32_t eax) {
    if(ebx == 1){

    }else if(ebx == 2){
        char ch = eax & 0xff;
        putchar(ch);
    }
    return;
}
