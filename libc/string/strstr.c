#include <string.h>
#include <stdio.h>

char* strstr(const char* str1, const char* str2) 
{
    int numMatches = 0;
    int requiredMatches = strlen(str2);
    char* currentCandidate = nullptr;
    const char* str2StartPos = str2;
	do
    {
        if(*str1 == '\0')
        {
            break;
        }

		if (*str1 == *str2)
        {
            if(!currentCandidate)
            {
                currentCandidate = (char*)str1;
            }

            numMatches++;
            if(numMatches == requiredMatches)
            {
			    return currentCandidate;
            }
        }
        else
        {
            numMatches = 0;
            currentCandidate = nullptr;
            str2 = str2StartPos - 1;
        }
        
    }
	while (*str1++ && *str2++);

	return nullptr;
}