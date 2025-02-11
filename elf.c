#include <elf.h>
#include <file.h>
#include <align.h>
#include <errors.h>
#include <page.h>
#include <cr.h>

// Used to store the elf before placing it at the elf entry point
#define DATA_AREA 0x100000

// Calculate size of elf in memory
static void calculate_mem_size(struct elf *elf_info)
{
  UINTN i;
  UINTN idx;
  for(i = 0; i < elf_info->elf_header.e_phnum; i++)
  {
    if(elf_info->program_header[i].p_type == PT_LOAD)
      idx = i;
  }

  UINTN size = elf_info->program_header[idx].p_paddr +
               elf_info->program_header[idx].p_memsz - elf_info->program_header[0].p_paddr;

  if(IS_ALIGN(size, PAGE_SIZE))
    size = ALIGNUP(size, PAGE_SIZE);

  elf_info->mem_size = size / PAGE_SIZE;
}

// Alloc and load program header of elf.
static EFI_STATUS load_program_header(struct elf *elf_info)
{
  EFI_STATUS status;

  UINTN size = elf_info->elf_header.e_phentsize * elf_info->elf_header.e_phnum;
  elf_info->program_header = bmalloc(size, EfiRuntimeServicesData);
  if(elf_info->program_header == NULL)
    return EFI_INVALID_PARAMETER;

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
  elf_info->string_table = bmalloc(size, EfiRuntimeServicesData);
  
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
  elf_info->section_header = bmalloc(size, EfiRuntimeServicesData);
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

// Map elf sections
static EFI_STATUS map_elf(struct elf *elf_info)
{
  cr_disable_wp_bit();
  if(EFI_ERROR(page_map_pages(elf_info->program_header[0].p_paddr, 1)))
    return EFI_INVALID_PARAMETER;
  
  memcpy((VOID *) elf_info->program_header[0].p_paddr, (VOID *) DATA_AREA, elf_info->mem_size * PAGE_SIZE);
  cr_enable_wp_bit();

  return EFI_SUCCESS;
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

  calculate_mem_size(elf_info);

  status = load_section_header(elf_info);
  if(EFI_ERROR(status))
    return status;

  return EFI_SUCCESS;
}

VOID elf_clear_all(struct elf *elf_info)
{
  elf_info->file_interface->Close(elf_info->file_interface);
}

EFI_STATUS elf_load_kernel(struct elf *elf_info)
{
  EFI_STATUS status;

  UINTN i;
  UINTN position;
  EFI_FILE_PROTOCOL *file_interface = elf_info->file_interface;
  EFI_PHYSICAL_ADDRESS address = DATA_AREA;
  UINT64 p_memsz, p_align;
  UINTN size;

  printf(L"elf_mem_size: %x\n", elf_info->mem_size);
  EFI_MEMORY_TYPE memory_type = EfiLoaderData;
  status = BS->AllocatePages(AllocateAddress,
                             memory_type,                    
                             elf_info->mem_size,               
                             &address);
  if(EFI_ERROR(status))      
  {
    error(L"Failed to alloc page to kernel.");
    return status;           
  }

  for(i = 0; i < elf_info->elf_header.e_phnum; i++)
  {
    if(elf_info->program_header[i].p_type != PT_LOAD)
      continue;

    p_memsz = elf_info->program_header[i].p_memsz;
    p_align = elf_info->program_header[i].p_align;

    if(IS_ALIGN(p_memsz, p_align))
      size = ALIGNUP(p_memsz, p_align);
    else
      size = p_memsz;

    address = DATA_AREA + (elf_info->program_header[i].p_paddr - 
                          elf_info->program_header[0].p_paddr);

    position = elf_info->program_header[i].p_offset;
    file_set_position(file_interface, position);
    if(file_read_file(file_interface, &size, (EFI_PHYSICAL_ADDRESS *) address))
      return EFI_UNSUPPORTED;
  }

  clear_bss(elf_info);
  if(EFI_ERROR(map_elf(elf_info)))
    return EFI_INVALID_PARAMETER;
  
  return EFI_SUCCESS;
}
