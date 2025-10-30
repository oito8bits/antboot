#ifndef _RAMFS_INCLUDE_
#define _RAMFS_INCLUDE_

#include <libefi.h>
#include <elf.h>

VOID *ramfs_init(EFI_FILE_PROTOCOL *, struct elf *);

#endif
