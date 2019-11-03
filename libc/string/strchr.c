#include <string.h>

char* strchr(char* str, int character) 
{
	do
    {
		if (*str == character)
        {
			return (char*)str;
        }
    }
	while (*str++);

	return 0;
}