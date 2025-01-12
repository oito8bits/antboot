#ifndef _PAGE_INCLUDE_
#define _PAGE_INCLUDE_

#include <efi.h>

UINT64 page_get_p4d_idx(EFI_VIRTUAL_ADDRESS address);
UINT64 page_get_pud_idx(EFI_VIRTUAL_ADDRESS address);
UINT64 page_get_pmd_idx(EFI_VIRTUAL_ADDRESS address);
UINT64 page_get_pte_idx(EFI_VIRTUAL_ADDRESS address);

VOID page_map_pages(EFI_PHYSICAL_ADDRESS *);

#endif
