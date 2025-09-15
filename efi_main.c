#include <efi.h>
#include <libefi.h>
#include <file.h>
#include <elf.h>
#include <memory.h>
#include <info.h>
#include <errors.h>
#include <page.h>

__attribute__((sysv_abi))
void (*_start_ant)(struct boot_info *);

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table)
{
  EFI_STATUS status;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *conout;
  init_lib(image_handle, system_table);

  conout = ST->ConOut;
  status = conout->SetMode(conout, 0);
  if(EFI_ERROR(status))
    error(L"SetMode: Failed to set mode. ");
 
  conout->SetAttribute(conout, EFI_BACKGROUND_BLACK | EFI_WHITE);
  
  EFI_FILE_PROTOCOL *file_protocol; 
  status = file_init_file_protocol(&file_protocol);
  if(EFI_ERROR(status))
    fatal_error(L"fat_init_file_protocol: Failed to init protocol.");
  
  EFI_FILE_PROTOCOL *kfile_interface;
  status = file_open_file(file_protocol, &kfile_interface, L"\\boot\\ant_kernel", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
  if(EFI_ERROR(status))
    fatal_error(L"fat_open_file: Failed to open file.");

  puts(L"Loading the elf kernel...");

  struct elf kernel_info;
  kernel_info.file_interface = kfile_interface;

  status = elf_parse(&kernel_info);
  if(EFI_ERROR(status))
    fatal_error(L"load_kernel: Failed to parse kernel.");

  page_init();
  status = elf_load_kernel(&kernel_info);
  if(EFI_ERROR(status))
    fatal_error(L"elf_load_kernel: Failed to load kernel.");

  struct boot_info *boot_info;
  boot_info = malloc(sizeof *boot_info);
  boot_info->kernel_entry = kernel_info.elf_header.e_entry;
  boot_info->kernel_size = kernel_info.mem_size * PAGE_SIZE;
  
  if((boot_info->smbios = getsmbios()) == NULL)
    error(L"getsmbios: Failed to find smbios.");
  
  if((boot_info->acpi = getacpi()) == NULL)
    error(L"getacpi: Failed to find acpi.");
  
  elf_clear_all(&kernel_info);
  
  status = gop_init(&boot_info->mode);
  if(EFI_ERROR(status))
    fatal_error(L"gop_init: Failed to init gop mode.");

  UINTN map_key;
  status = memory_get_map(&boot_info->map, &map_key);
  if(EFI_ERROR(status))
    fatal_error(L"memory_get_map: Failed to get memory map.");
  
  status = BS->ExitBootServices(IH, map_key);
  if(EFI_ERROR(status))
    fatal_error(L"ExitBootServices: Failed to exit boot services.");
  
  _start_ant = (__attribute__((sysv_abi)) void (*)(struct boot_info *)) 
               kernel_info.elf_header.e_entry;
  _start_ant(boot_info);

  return EFI_SUCCESS;
}
