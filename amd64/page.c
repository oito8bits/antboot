#include <page.h>
#include <cr3.h>

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

VOID page_map_pages(EFI_PHYSICAL_ADDRESS *address)
{
  
}
