#ifndef _STDIO_H
#define _STDIO_H 1
 
#include <sys/cdefs.h>
 
#define EOF (-1)
 
#ifdef __cplusplus
extern "C" {
#endif
 
int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);

void itoa(unsigned i, unsigned base, char* buf);
void itoa_s(int i, unsigned base, char* buf);
int atoi(const char* str);
#ifdef __cplusplus
}
#endif
 
#endif