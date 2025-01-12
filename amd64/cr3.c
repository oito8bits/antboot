#include <cr3.h>

UINT64 cr3_get(VOID)
{
  UINT64 rax;
  __asm__ __volatile__("movq  %%cr3, %%rax" : "=a" (rax));
  
  return rax;
}
