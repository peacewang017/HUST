// See LICENSE for license details.

#include "string.h"

#include <ctype.h>
#include <stdint.h>

void* memcpy(void* dest, const void* src, size_t len) {
  const char* s = src;
  char* d = dest;

  if ((((uintptr_t)dest | (uintptr_t)src) & (sizeof(uintptr_t) - 1)) == 0) {
    while ((void*)d < (dest + len - (sizeof(uintptr_t) - 1))) {
      *(uintptr_t*)d = *(const uintptr_t*)s;
      d += sizeof(uintptr_t);
      s += sizeof(uintptr_t);
    }
  }

  while (d < (char*)(dest + len)) *d++ = *s++;

  return dest;
}

void* memset(void* dest, int byte, size_t len) {
  if ((((uintptr_t)dest | len) & (sizeof(uintptr_t) - 1)) == 0) {
    uintptr_t word = byte & 0xFF;
    word |= word << 8;
    word |= word << 16;
    word |= word << 16 << 16;

    uintptr_t* d = dest;
    while (d < (uintptr_t*)(dest + len)) *d++ = word;
  } else {
    char* d = dest;
    while (d < (char*)(dest + len)) *d++ = byte;
  }
  return dest;
}

size_t strlen(const char* s) {
  const char* p = s;
  while (*p) p++;
  return p - s;
}

int strcmp(const char* s1, const char* s2) {
  unsigned char c1, c2;

  do {
    c1 = *s1++;
    c2 = *s2++;
  } while (c1 != 0 && c1 == c2);

  return c1 - c2;
}

char* strcpy(char* dest, const char* src) {
  char* d = dest;
  while ((*d++ = *src++))
    ;
  return dest;
}

char *strchr(const char *p, int ch)
{
	char c;
	c = ch;
	for (;; ++p) {
		if (*p == c)
			return ((char *)p);
		if (*p == '\0')
			return (NULL);
	}
}

char* strtok(char* str, const char* delim) {
  static char* current;
  if (str != NULL) current = str;
  if (current == NULL) return NULL;

  char* start = current;
  while (*start != '\0' && strchr(delim, *start) != NULL) start++;

  if (*start == '\0') {
    current = NULL;
    return current;
  }

  char* end = start;
  while (*end != '\0' && strchr(delim, *end) == NULL) end++;

  if (*end != '\0') {
    *end = '\0';
    current = end + 1;
  } else
    current = NULL;
  return start;
}

char *strcat(char *dst, const char *src) {
  strcpy(dst + strlen(dst), src);
  return dst;
}

long atol(const char* str) {
  long res = 0;
  int sign = 0;

  while (*str == ' ') str++;

  if (*str == '-' || *str == '+') {
    sign = *str == '-';
    str++;
  }

  while (*str) {
    res *= 10;
    res += *str++ - '0';
  }

  return sign ? -res : res;
}

void* memmove(void* dst, const void* src, size_t n) {
  const char* s;
  char* d;

  s = src;
  d = dst;
  if (s < d && s + n > d) {
    s += n;
    d += n;
    while (n-- > 0) *--d = *--s;
  } else
    while (n-- > 0) *d++ = *s++;

  return dst;
}

// Like strncpy but guaranteed to NUL-terminate.
char* safestrcpy(char* s, const char* t, int n) {
  char* os;

  os = s;
  if (n <= 0) return os;
  while (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  return os;
}

void strncpy(char *dst, const char *src, size_t n){
  size_t i;

  // 复制 src 的字符到 dest，直到达到 n 或者遇到 '\0'
  for(i = 0; i < n && src[i] != '\0'; i++){
    dst[i] = src[i];
  }

  // 如果 src 长度小于 n，补充 '\0'
  for(; i < n; i++){
    dst[i] = '\0';
  }
}

void str_insert_to_head(char* dst, const char* src){
  int max_length = strlen(dst) + strlen(src) + 1;
  char temp[max_length];
  temp[0] = '\0';
  strcat(temp, src);
  strcat(temp, dst);
  strcpy(dst, temp);
  return;
}

// 主要分为两类情况
// 有token，直接分开
// 无token，dst2 == src，dst1 == "."
void str_div_by_token(const char* src, char* dst1, char* dst2){
  char* p = (char*)src + strlen(src) - 1;
  while(p >= src && *(p - 1) != '/'){ // 移动到 / 的位置
    p--;
  }
  if(p < src){
    dst1[0] = '.';
    dst1[1] = '\0';
    strcpy(dst2, src);
    return;
  }
  strcpy(dst2, p);
  int length_dst1 = p - src;
  strncpy(dst1, src, length_dst1);
  dst1[length_dst1] = '\0';
  return;
}