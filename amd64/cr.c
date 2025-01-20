#include <cr.h>

VOID cr_set_cr0(UINT64 cr0) {
  __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}

UINT64 cr_get_cr0(VOID) {
  UINT64 cr0;
  __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));

  return cr0;
}

VOID cr_disable_wp_bit(VOID) { 
  UINT64 cr0 = cr_get_cr0();

  cr0 &= ~(1UL << 16);

  cr_set_cr0(cr0);
}

UINT64 cr_get_cr3(VOID)
{
  UINT64 rax;
  __asm__ __volatile__("movq  %%cr3, %%rax" : "=a" (rax));

  return rax;
}
