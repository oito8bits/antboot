#include <elf.h>
#include <file.h>
#include <align.h>
#include <errors.h>

// Alloc and load program header of elf.
static EFI_STATUS load_program_header(struct elf *elf_info)
{
  EFI_STATUS status;

  UINTN size = elf_info->elf_header.e_phentsize * elf_info->elf_header.e_phnum;
  elf_info->program_header = allocate_boot_pool(size);
  if(elf_info->program_header == NULL)
    return EFI_UNSUPPORTED;

  EFI_FILE_PROTOCOL *file_interface = elf_info->file_interface;
  file_set_position(file_interface, elf_info->elf_header.e_phoff);
  status = file_read_file(file_interface, &size, elf_info->program_header);
  if(EFI_ERROR(status))
    return status;

  return EFI_SUCCESS;
} 

// Load string table of elf.
static EFI_STATUS load_string_table(struct elf *elf_info)
{
  EFI_STATUS status;

  struct elf_64_section_header str_section = elf_info->section_header[elf_info->elf_header.e_shstrndx];

  UINTN size = str_section.sh_size;
  elf_info->string_table = allocate_boot_pool(size);
  
  EFI_FILE_PROTOCOL *file_interface = elf_info->file_interface;
  file_set_position(file_interface, str_section.sh_offset);
  status = file_read_file(file_interface, &size, elf_info->string_table);
  if(EFI_ERROR(status))
    return status;

  return EFI_SUCCESS;
}

// Search section and return index
static INTN search_section_index(struct elf *elf_info, char *name)
{
  INTN i;
  for(i = 0; i < elf_info->elf_header.e_shnum; i++)
  {
    struct elf_64_section_header section_header = elf_info->section_header[i];
    char *section_name = elf_info->string_table + section_header.sh_name;
    if(!memcmp(name, section_name, 4))
      return i;
  }

  return -1;
}

// Clear bss section with zeros.
static VOID clear_bss(struct elf *elf_info)
{
  UINTN i, j;
  char *bss_name = ".bss";
  INTN section_index = search_section_index(elf_info, bss_name);

  if(section_index < 0)
    return;

  struct elf_64_section_header section_header = elf_info->section_header[section_index];
  UINT64 paddr = section_header.sh_addr - elf_info->program_header[0].p_vaddr +
                 elf_info->program_header[0].p_paddr;
  memset((VOID *) paddr, 0, section_header.sh_size);
}

// Alloc and load section header of elf.
static EFI_STATUS load_section_header(struct elf *elf_info)
{
  EFI_STATUS status;

  UINTN size = elf_info->elf_header.e_shentsize * elf_info->elf_header.e_shnum;  
  elf_info->section_header = allocate_boot_pool(size);
  if(elf_info->section_header == NULL)
    return EFI_UNSUPPORTED;

  EFI_FILE_PROTOCOL *file_interface = elf_info->file_interface;
  file_set_position(file_interface, elf_info->elf_header.e_shoff);
  status = file_read_file(file_interface, &size, elf_info->section_header);
  if(EFI_ERROR(status))
    return status;

  status = load_string_table(elf_info);
  if(EFI_ERROR(status))
    return status;

  return EFI_SUCCESS;
}

// Check if file is an elf.
static INTN is_elf(struct elf *elf_info)
{
  return *((UINT32 *) &elf_info->elf_header.e_ident) == 0x464c457f;
}

EFI_STATUS elf_parse(struct elf *elf_info)
{
  EFI_STATUS status;
  EFI_FILE_PROTOCOL *file_interface = elf_info->file_interface;

  UINTN size = sizeof(struct elf_64_header);
  status = file_read_file(file_interface, &size, &elf_info->elf_header);
  if(EFI_ERROR(status))
    return status;

  if(!is_elf(elf_info))
    return EFI_UNSUPPORTED;

  status = load_program_header(elf_info);
  if(EFI_ERROR(status))
    return status;

  status = load_section_header(elf_info);
  if(EFI_ERROR(status))
    return status;

  return EFI_SUCCESS;
}

VOID elf_clear_all(struct elf *elf_info)
{
  elf_info->file_interface->Close(elf_info->file_interface);
  free_pool(elf_info->program_header);
  free_pool(elf_info->section_header);
  free_pool(elf_info->string_table);
}

EFI_STATUS elf_load_kernel(struct elf *elf_info)
{
  EFI_STATUS status;

  UINTN i;
  UINTN size;
  UINTN position;
  EFI_FILE_PROTOCOL *file_interface = elf_info->file_interface;
  EFI_PHYSICAL_ADDRESS address;
  UINT64 p_memsz, p_align;
  EFI_MEMORY_TYPE memory_type;
  for(i = 0; i < elf_info->elf_header.e_phnum; i++)
  {
    if(elf_info->program_header[i].p_type != PT_LOAD)
      continue;

    p_memsz = elf_info->program_header[i].p_memsz;
    p_align = elf_info->program_header[i].p_align;

    if(p_memsz & p_align - 1)
      size = ALIGNUP(p_memsz, p_align);
    else
      size = p_memsz;

    address = elf_info->program_header[i].p_paddr;
    if(elf_info->program_header[i].p_flags & 0x1)
      memory_type = EfiRuntimeServicesCode;
    else
      memory_type = EfiRuntimeServicesData;

    status = BS->AllocatePages(AllocateAddress,
                               memory_type,
                               size / PAGE_SIZE,
                               &address);

    if(EFI_ERROR(status))
    {
      error(L"Failed to alloc page to kernel.");
      return status;
    }

    position = elf_info->program_header[i].p_offset;
    file_set_position(file_interface, position);
    if(file_read_file(file_interface, &size, (EFI_PHYSICAL_ADDRESS *) address))
      return EFI_UNSUPPORTED;
  }

  clear_bss(elf_info);

  struct elf_64_section_header boot_info_section = elf_info->section_header[search_section_index(elf_info, ".boot_info")];
  elf_info->boot_info_addr = (VOID *) boot_info_section.sh_addr - 
                             elf_info->program_header[0].p_vaddr + 
                             elf_info->program_header[0].p_paddr;
  return EFI_SUCCESS;
}
