#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

void *memcpy(void* dest, const void* src, size_t len);
void *memset(void* dest, int byte, size_t len);
size_t strlen(const char* s);
int strcmp(const char* s1, const char* s2);
char *strcpy(char* dest, const char* src);
char *strchr(const char *p, int ch);
char *strtok(char* str, const char* delim);
char *strcat(char *dst, const char *src);
long atol(const char* str);
void *memmove(void* dst, const void* src, size_t n);
char *safestrcpy(char* s, const char* t, int n);
void strncpy(char *dst, const char *src, size_t n);
void str_insert_to_head(char* dst, const char* src);
void str_div_by_token(const char* src, char* dst1, char* dst2);

#endif
