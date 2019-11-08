#include <string.h>

char* strcpy(char *str1, const char *str2)
{
    char* s1_p = str1;
    while ((*str1++ = *str2++));
    return s1_p;
}