#ifndef _MEMORY_INCLUDE_
#define _MEMORY_INCLUDE_

#include <libefi.h>
#define PAGE_SIZE 4096

struct memory_map
{
  UINTN memory_map_size;
  EFI_MEMORY_DESCRIPTOR *memory_descriptor;
  UINTN descriptor_size;
};

EFI_STATUS memory_get_map(struct memory_map *map, UINTN *map_key);

#endif
