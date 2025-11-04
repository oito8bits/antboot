#include <file.h>

// Init file protocol.
EFI_STATUS file_init_file_protocol(EFI_FILE_PROTOCOL **root)
{
  EFI_STATUS status;

  EFI_GUID fs_protocol_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *device;
  status = BS->LocateProtocol(&fs_protocol_guid,
                              NULL,
                              (VOID **) &device);

  if(EFI_ERROR(status))
    return status;
  
  EFI_FILE_PROTOCOL *rootp;
  status = device->OpenVolume(device, &rootp);
  if(EFI_ERROR(status))
    return status;

  *root = rootp;

  return EFI_SUCCESS;
}

// Open a file from fat partition.
EFI_STATUS file_open_file(EFI_FILE_PROTOCOL *root,
                          EFI_FILE_PROTOCOL **file_interface,
                          CHAR16 *pathname,
                          UINT64 open_mode, 
                          UINT64 attributes)
{
  EFI_STATUS status;
  status = root->Open(root, file_interface, pathname, open_mode, attributes);
  
  if(EFI_ERROR(status))
    return status;

  return EFI_SUCCESS;
}

// Close file.
EFI_STATUS file_close_file(EFI_FILE_PROTOCOL *file_interface)
{
  file_interface->Close(file_interface);
}

// Set the byte position.
EFI_STATUS file_set_position(EFI_FILE_PROTOCOL *file_interface, UINT64 position)
{
  EFI_STATUS status;
  file_interface->SetPosition(file_interface, position);

  if(EFI_ERROR(status))
    return status;

  return EFI_SUCCESS;
}

// Read a file.
EFI_STATUS file_read_file(EFI_FILE_PROTOCOL *file_interface,
                          UINTN *buffer_size,
                          VOID *buffer)
{
  EFI_STATUS status;
  status = file_interface->Read(file_interface, buffer_size, buffer);
  if(EFI_ERROR(status))
    return status;

  return EFI_SUCCESS;
}

// Get file info.
EFI_STATUS file_get_info(EFI_FILE_PROTOCOL *file_interface,
                         EFI_FILE_INFO *file_info)
{
  EFI_GUID file_info_guid = EFI_FILE_INFO_ID;
  UINTN size = sizeof(EFI_FILE_INFO);
  return file_interface->GetInfo(file_interface, &file_info_guid, &size, file_info);
}
