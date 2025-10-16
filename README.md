# antboot

UEFI Bootloader for x86-64 architecture. This is submodule of [Ant](https://github.com/8oito-bits/Ant) repository.

![](ant.png)

# How to build

You will need:
`x86_64-w64-mingw32-gcc` (install `gcc-mingw-w64`)
`make`
`as` (Gnu Assembler)

# How to use

To use, simply create a FAT partition and place BOOTX64.EFI in the efi/boot/ directory and the kernel elf in boot/. A usage example can be seen [here](https://github.com/oito8bits/Ant).

A structure pointer will be passed to the kernel through the RDI register. The structure is as follows:
```c
struct boot_info
{
  struct memory_map map;    
  struct gop_video_mode mode;
  UINT64 kernel_entry;
  UINT64 kernel_size;
  VOID *smbios;
  VOID *acpi;
};
```
