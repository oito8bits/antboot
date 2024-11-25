#include <libefi.h>

INTN memcmp(const VOID *buf1, const VOID *buf2, INTN count)
{
  const UINT8 *p = buf1;
  const UINT8 *q = buf2;

  while(count--)
  { 
    if(*p++ != *q++)
      return p[-1] < q[-1] ? -1 : 1;  
  } 
    
  return 0;
}

VOID *memcpy(VOID *dest, const VOID *src, UINT64 n)
{
  UINT8 *p = dest;
  const UINT8 *q = src;

  while(n--)
    *p++ = *q++;;

  return dest;
}

UINTN strlen(const CHAR16 *str)
{
  const CHAR16 *p = str - 1;

  while(*++p);

  return p - str;
}

VOID *memset(VOID *s, UINTN c, UINTN size)
{
  UINT8 *p = s;
  while(size-- > 0)
    *p++ = c;

  return s;
}
