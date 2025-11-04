#include <ramfs.h>
#include <file.h>
#include <memory.h>

EFI_STATUS ramfs_init(EFI_FILE_PROTOCOL *root, struct ramfs *ramfs, struct elf *elf_info)
{
  EFI_STATUS status;
  EFI_FILE_PROTOCOL *file_interface;
  status = file_open_file(root,
                          &file_interface,
                          L"ramfs.tar",
                          EFI_FILE_MODE_READ,
                          EFI_FILE_READ_ONLY);
  if(EFI_ERROR(status))
    return status;

  EFI_FILE_INFO file_info;
  status = file_get_info(file_interface, &file_info);
  if(EFI_ERROR(status))
    return status;

  UINTN file_size = file_info.FileSize;
  EFI_PHYSICAL_ADDRESS address = elf_info->program_header[0].p_paddr + 
                                 elf_info->mem_size * PAGE_SIZE;
  status = BS->AllocatePages(AllocateAddress,
                             EfiLoaderData,
                             file_size / PAGE_SIZE,
                             &address);
  if(EFI_ERROR(status))
    return status;
  
  file_read_file(file_interface, &file_size, (void *) address);
  
  ramfs->base = (void *) address;
  ramfs->size = file_size;  

  return EFI_SUCCESS;
}
