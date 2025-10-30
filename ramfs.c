#include <ramfs.h>
#include <file.h>
#include <memory.h>

VOID *ramfs_init(EFI_FILE_PROTOCOL *root, struct elf *elf_info)
{
  EFI_STATUS status;
  EFI_FILE_PROTOCOL *file_interface;
  status = file_open_file(root,
                          &file_interface,
                          L"ramfs.tar",
                          EFI_FILE_MODE_READ,
                          EFI_FILE_READ_ONLY);
  if(EFI_ERROR(status))
    return NULL;

  EFI_FILE_INFO file_info;
  status = file_get_info(file_interface, &file_info);
  if(EFI_ERROR(status))
    return NULL;

  EFI_PHYSICAL_ADDRESS address = elf_info->program_header[0].p_paddr + 
                                 elf_info->mem_size * PAGE_SIZE;
  status = BS->AllocatePages(AllocateAddress,
                             EfiLoaderData,
                             file_info.FileSize,
                             &address);
  if(EFI_ERROR(status))
    return NULL;

  
  return (void *) address;
}
