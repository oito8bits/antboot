#ifndef _PAGE_INCLUDE_
#define _PAGE_INCLUDE_

#include <libefi.h>

#define PAGE_2MIB (2 << 20)
#define PAGE_SIZE PAGE_2MIB

UINT64 page_get_l4_idx(EFI_VIRTUAL_ADDRESS address);
UINT64 page_get_l3_idx(EFI_VIRTUAL_ADDRESS address);
UINT64 page_get_l2_idx(EFI_VIRTUAL_ADDRESS address);
UINT64 page_get_l1_idx(EFI_VIRTUAL_ADDRESS address);

VOID page_map_pages(EFI_PHYSICAL_ADDRESS, UINTN);
VOID page_init(VOID);

#endif
