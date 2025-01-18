#include <page.h>
#include <cr3.h>

UINT64 *pmd = 0;

UINT64 page_get_p4d_idx(EFI_VIRTUAL_ADDRESS address)
{
  return address >> 39 & 0x1ff;
}

UINT64 page_get_pud_idx(EFI_VIRTUAL_ADDRESS address)
{ 
  return address >> 30 & 0xff;
}

UINT64 page_get_pmd_idx(EFI_VIRTUAL_ADDRESS address)
{
  return address >> 21 & 0x1ff;
}

UINT64 page_get_pte_idx(EFI_VIRTUAL_ADDRESS address)
{
  return address >> 12 & 0x1ff;
}

VOID page_map_pages(EFI_PHYSICAL_ADDRESS address_base, UINTN npages)
{
  UINTN i;
  for(i = 0; i < npages; i += PAGE_2MIB)
    pmd[page_get_pmd_idx(address_base + i)] = address_base + i + 0x83;
}

VOID page_init(VOID)
{
  UINT64 *p4d = (UINT64 *) cr3_get();
  UINT64 *pud = (UINT64 *) (p4d[0] & ~0xfff);
  
  pmd = (UINT64 *) (pud[0] & ~0xfff);
  printf(L"pmd %x, pud: %x, p4d: %x\r\n", pmd, pud, p4d[0]);
}
