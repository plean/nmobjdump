/*
** my_objdump.c for  in /home/planch_j/rendu/c/PSU_2016_nmobjdump
** 
** Made by Jean PLANCHER
** Login   <planch_j@epitech.net>
** 
** Started on  Ven Feb 24 21:42:42 2017 Jean PLANCHER
** Last update Ven Feb 24 21:42:42 2017 Jean PLANCHER
*/

#include "nmobjdump.h"
#include "bfd_seen.h"

static char const *g_flags[10] = {
  "BFD_NO_FLAGS, ",
  "HAS_RELOC, ",
  "EXEC_P, ",
  "HAS_LINENO, ",
  "HAS_DEBUG, ",
  "HAS_SYMS, ",
  "HAS_LOCALS, ",
  "DYNAMIC, ",
  "WP_TEXT, ",
  "D_PAGED, ",
};

/*
** every time i'm using raw number instead of elf macro here it's because
** of the stupide norm and it's 80 cols/line, 25 lines/func, 5 funcs/file
*/

#ifdef ELFCLASS_32
int			my_objdump_get_flags_32(void const *mapped, Elf32_Ehdr const *elf)
{
  Elf32_Shdr const	*sect;
#elif defined(ELFCLASS_64)
int			my_objdump_get_flags_64(void const *mapped, Elf64_Ehdr const *elf)
{
  Elf64_Shdr const	*sect;
#endif
  int			flags;
  int			i;

  fflush(stdout);
  flags = 0;
  i = 0;
  while (i < elf->e_shnum)
    {
      sect = mapped + (elf->e_shoff + elf->e_shentsize * i++);
      flags |=
	HAS_RELOC * (elf->e_type == ET_REL &&
		     (sect->sh_type == SHT_RELA || sect->sh_type == SHT_REL)) |
	EXEC_P * (elf->e_type == ET_EXEC) |
	HAS_SYMS * (sect->sh_type == SHT_SYMTAB ||
		    sect->sh_type == SHT_DYNSYM) |
	DYNAMIC * (elf->e_type == ET_DYN) |
	D_PAGED * ((elf->e_type == ET_EXEC || elf->e_type == ET_DYN));
    }
  fflush(stdout);
  return (flags);
}

#ifdef ELFCLASS_32
void		my_objdump_32_disp_header(void const *mapped,
					  Elf32_Ehdr const *elf,
					  char const *filename)
#elif defined(ELFCLASS_64)
void		my_objdump_64_disp_header(void const *mapped,
					  Elf64_Ehdr const *elf,
					  char const *filename)
#endif
{
  char		*buffer;
  int		flags;
  int		i;

  buffer = strdup("");
#ifdef ELFCLASS_32
  flags = my_objdump_get_flags_32(mapped, elf);
#elif defined(ELFCLASS_64)
  flags = my_objdump_get_flags_64(mapped, elf);
#endif
  i = -1;
  while (++i < 11)
    if (flags >> i & 1 &&
	asprintf(&buffer, "%s%s", buffer, g_flags[i + 1]));
  buffer[MAX(strlen(buffer), 2) - 2] = 0;
#ifdef ELFCLASS_64
  printf("\n%s:     file format elf64-x86-64\narchitecture: %s, flags 0x%08x:"
	 "\n%s\nstart address 0x%016lx\n\n", filename, MCHN_64(elf->e_machine),
	 flags, buffer, elf->e_entry);
#elif defined(ELFCLASS_32)
  printf("\n%s:     file format elf32-i386\narchitecture: %s, flags 0x%08x:"
	 "\n%s\nstart address 0x%08x\n\n", filename, MCHN_32(elf->e_machine),
	 flags, buffer, elf->e_entry);
#endif
  free(buffer);
}

#ifdef ELFCLASS_32
void			my_objdump_32(void const *mapped, Elf32_Ehdr const *elf,
			      char const *filename)
{
  Elf32_Shdr const	*s;
#elif defined(ELFCLASS_64)
void			my_objdump_64(void const *mapped, Elf64_Ehdr const *elf,
			      char const *filename)
{
  Elf64_Shdr const	*s;
#endif
  char const		*b;
  int			i;

#ifdef ELFCLASS_64
  my_objdump_64_disp_header(mapped, elf, filename);
#elif defined(ELFCLASS_32)
  my_objdump_32_disp_header(mapped, elf, filename);
#endif
  s = mapped + (elf->e_shstrndx * elf->e_shentsize + elf->e_shoff);
  b = malloc(sizeof(char) * s->sh_size);
  b = mapped + (s->sh_offset);
  i = -1;
  while (++i < elf->e_shnum)
    {
      s = mapped + (elf->e_shoff + i * elf->e_shentsize);
      if (s->sh_size && s->sh_type != SHT_NOBITS && s->sh_type != SHT_SYMTAB &&
	  (s->sh_type != SHT_STRTAB || !strcmp(".dynstr", b + s->sh_name)) &&
	  ((s->sh_flags & SHF_ALLOC) ||
	   (s->sh_type != SHT_RELA && s->sh_type != SHT_REL)))
	print_section(mapped + s->sh_offset, s->sh_size, s->sh_addr,
		      b + s->sh_name);
    }
}
