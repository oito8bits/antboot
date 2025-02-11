#include <memory.h>

// Get memory map.
EFI_STATUS memory_get_map(struct memory_map *map, UINTN *map_key)
{
  EFI_STATUS status;

  UINTN memory_map_size = 0;
  EFI_MEMORY_DESCRIPTOR *memory_descriptor;
  UINTN descriptor_size;
  UINT32 descriptor_version;
  BS->GetMemoryMap(&memory_map_size, memory_descriptor, map_key, &descriptor_size, &descriptor_version);

  memory_descriptor = bmalloc(memory_map_size, EfiRuntimeServicesData);
  
  status = BS->GetMemoryMap(&memory_map_size, memory_descriptor, map_key, &descriptor_size, &descriptor_version);

  if(EFI_ERROR(status))
    return status;

  map->memory_map_size = memory_map_size;
  map->memory_descriptor = memory_descriptor;
  map->descriptor_size = descriptor_size;

  return EFI_SUCCESS;
}
