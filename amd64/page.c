#include <page.h>
#include <align.h>
#include <cr.h>

UINT64 *pmd = 0;

UINT64 page_get_l4_idx(EFI_VIRTUAL_ADDRESS address)
{
  return address >> 39 & 0x1ffUL;
}

UINT64 page_get_l3_idx(EFI_VIRTUAL_ADDRESS address)
{ 
  return address >> 30 & 0x1ffUL;
}

UINT64 page_get_l2_idx(EFI_VIRTUAL_ADDRESS address)
{
  return address >> 21 & 0x1ffUL;
}

UINT64 page_get_l1_idx(EFI_VIRTUAL_ADDRESS address)
{
  return address >> 12 & 0x1ffUL;
}

EFI_STATUS page_map_pages(EFI_PHYSICAL_ADDRESS address_base, UINTN npages)
{
  if(IS_ALIGN(address_base, PAGE_2MIB))
    return EFI_INVALID_PARAMETER;

  UINTN i;
  for(i = 0; i < npages * PAGE_2MIB; i += PAGE_2MIB);
    pmd[page_get_l2_idx(address_base + i)] = address_base + i + 0x83;

  return EFI_SUCCESS;
}

VOID page_init(VOID)
{
  UINT64 *p4d = (UINT64 *) cr_get_cr3();
  UINT64 *pud = (UINT64 *) (p4d[0] & ~0xfffUL);
  
  pmd = (UINT64 *) (pud[0] & ~0xfffUL);
}
