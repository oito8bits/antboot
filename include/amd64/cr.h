#ifndef _CR_INCLUDE_
#define _CR_INCLUDE_

#include <efi.h>

VOID cr_set_cr0(UINT64 cr0);
UINT64 cr_get_cr0(VOID);
VOID cr_disable_wp_bit(VOID);
VOID cr_enable_wp_bit(VOID);

UINT64 cr_get_cr3(VOID);

#endif
