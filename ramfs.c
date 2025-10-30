#include <ramfs.h>
#include <file.h>

VOID *ramfs_init(EFI_FILE_PROTOCOL *root)
{
  EFI_STATUS status;
  EFI_FILE_PROTOCOL *file_interface;
  status = file_open_file(root, &file_interface, L"ramfs.img", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
  if(EFI_ERROR(status))
    return NULL;
  //status = BS->AllocatePages();
  return NULL;
}
