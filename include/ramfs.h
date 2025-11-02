#ifndef _RAMFS_INCLUDE_
#define _RAMFS_INCLUDE_

#include <libefi.h>
#include <elf.h>

struct ramfs
{
  VOID *base;
  UINT64 size;
};

EFI_STATUS ramfs_init(EFI_FILE_PROTOCOL *, struct ramfs *, struct elf *);

#endif
