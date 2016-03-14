#include <string.h>
#include <stdint.h>
#include <stddef.h>
int strlen(const char* str) {
    if(str == NULL)
        return 0;

    uint32_t cnt = 0;
    while(str[cnt]) cnt++;
    return cnt;
}

bool streq(const char* str1, const char* str2) {
    uint32_t len1 = strlen(str1);
    uint32_t len2 = strlen(str2);
    if(len1 != len2)
        return false;
    else
        for(uint32_t i = 0; i < len1; i++) {
            if(str1[i] != str2[i])
                return false;
        }
    return true;
}
