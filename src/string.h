#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

int strlen(const char* str);
bool streq(const char* str1, const char* str2);
bool streqn(const char* str1, const char* str2, uint32_t n);
#endif // STRING_H_INCLUDED
