#ifndef _ELF_INCLUDE_
#define _ELF_INCLUDE_

#include <libefi.h>


// Used by e_type.
#define PT_NULL   0   // Program header table entry unused
#define PT_LOAD   1   // Loadable program segment

#define EI_NIDENT 16

// Elf Header Structure for AMD64/x86_64.
struct elf_64_header
{
  unsigned char e_ident[EI_NIDENT];
  UINT16        e_type;
  UINT16        e_machine;
  UINT32        e_version;
  UINT64        e_entry;
  UINT64        e_phoff;
  UINT64        e_shoff;
  UINT32        e_flags;
  UINT16        e_ehsize;
  UINT16        e_phentsize;
  UINT16        e_phnum;
  UINT16        e_shentsize;
  UINT16        e_shnum;
  UINT16        e_shstrndx;
};

// Program Header Structure for AMD64/x86_64.
struct elf_64_program_header
{
  UINT32  p_type;
  UINT32  p_flags;
  UINT64  p_offset;
  UINT64  p_vaddr;
  UINT64  p_paddr;
  UINT64  p_filesz;
  UINT64  p_memsz;
  UINT64  p_align;
};

// Section Header Structure for AMD64/x86_64.
struct elf_64_section_header
{
  UINT32  sh_name;
  UINT32  sh_type;
  UINT64  sh_flags;
  UINT64  sh_addr;
  UINT64  sh_offset;
  UINT64  sh_size;
  UINT32  sh_link;
  UINT32  sh_info;
  UINT64  sh_addralign;
  UINT64  sh_entsize;
};

// Info of elf file.
struct elf
{
  EFI_FILE_PROTOCOL             *file_interface;
  struct elf_64_header          elf_header;
  struct elf_64_program_header  *program_header;
  struct elf_64_section_header  *section_header;
  char                          *string_table;
  UINTN                         mem_size;
};

EFI_STATUS elf_parse(struct elf *);
VOID elf_clear_all(struct elf *elf_info);
EFI_STATUS elf_load_kernel(struct elf *);

#endif
