#include <string.h>
#include <stdint.h>
#include <stddef.h>
int strlen(const char* str) {
    if(str == NULL)
        return 0;

    size_t cnt = 0;
    while(str[cnt]) cnt++;
    return cnt;
}
