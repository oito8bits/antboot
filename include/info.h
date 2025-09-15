#ifndef _INFO_INCLUDE_
#define _INFO_INCLUDE_

#include <libefi.h>
#include <memory.h>
#include <gop.h>

struct boot_info
{
  struct memory_map map;    
  struct gop_video_mode mode;
  UINT64 kernel_entry;
  UINT64 kernel_size;
  VOID *smbios;
  VOID *acpi;
};

#endif
