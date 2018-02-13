/*
** my_nm.c for  in /home/planch_j/rendu/c/PSU_2016_nmobjdump
** 
** Made by Jean PLANCHER
** Login   <planch_j@epitech.net>
** 
** Started on  Ven Feb 24 21:42:42 2017 Jean PLANCHER
** Last update Ven Feb 24 21:42:42 2017 Jean PLANCHER
*/

#include "nmobjdump.h"

#ifdef ELFCLASS_64
void	disp_symbs(t_symbols const *sym, int i)
{
  int	a;

  a = -1;
  while (++a < i)
    {

      if (sym[a].print_val)
	printf("                ");
      else
	printf("%016x", sym[a].value);
      printf(" %c ", sym[a].type);
      printf("%s\n", sym[a].name);
    }
}

void		sort_symbs(t_symbols *sym, int i)
{
  t_symbols	c;
  char		*str1;
  char		*str2;
  int		a;
  int		b;

  a = -1;
  while (++a < i)
    {
      b = -1;
      while (++b < i)
	{
	  str1 = remove_char_from_string(sym[a].name, '_');
	  str2 = remove_char_from_string(sym[b].name, '_');
	  if (strcasecmp(str1, str2) < 0)
	    {
	      memcpy(&c, sym + a, sizeof(c));
	      memcpy(sym + a, sym + b, sizeof(c));
	      memcpy(sym + b, &c, sizeof(c));
	    }
	  free(str1);
	  free(str2);
	}
    }
}
#endif

#ifdef ELFCLASS_32
char        get_type_32(Elf32_Ehdr const *elf,
			Elf32_Shdr const *section,
			Elf32_Sym const *sym)
#elif defined(ELFCLASS_64)
char        get_type_64(Elf64_Ehdr const *elf,
			Elf64_Shdr const *section,
			Elf64_Sym const *sym)
#endif
{
  char	type;

  if (sym->st_shndx == SHN_ABS && (type = 'a'));
  else if (ELF64_ST_BIND(sym->st_info) == STB_WEAK)
    return (!sym->st_value ?
    	    (ELF64_ST_TYPE(sym->st_info) == STT_OBJECT ? 'v' : 'w') :
    	    (ELF64_ST_TYPE(sym->st_info) == STT_OBJECT ? 'V' : 'W'));
  else if (sym->st_shndx == SHN_COMMON)
    return ('C');
  else if (sym->st_shndx == SHN_UNDEF)
    return ('U');
  else if (IS_FROM_SECTION(".debug"))
    return ('N');
  else if (IS_FROM_SECTION(".bss") && (type = 'b'));
  else if (IS_FROM_SECTION(".text")
  	   || ELF64_ST_TYPE(sym->st_info) == STT_FUNC)
    type = 't';
  else if (IS_FROM_SECTION(".rodata") && (type = 'r'));
  else if (ELF64_ST_TYPE(sym->st_info) == STT_OBJECT
	   || ELF64_ST_TYPE(sym->st_info) == STT_NOTYPE
	   || IS_FROM_SECTION(".data") || IS_FROM_SECTION(".data1"))
    type = 'd';
  else
    return ('?');
  return (ELF64_ST_BIND(sym->st_info) == STB_LOCAL ? type : type - 32);
}

#ifdef ELFCLASS_32
void			parse_symtab_32(void const *mapped,
					Elf32_Ehdr const *elf,
					Elf32_Shdr const *section)
{
  Elf32_Sym const	*symb_header;
  t_symbols		sym[section->sh_size / sizeof(Elf32_Sym)];
#elif defined(ELFCLASS_64)
void			parse_symtab_64(void const *mapped,
					Elf64_Ehdr const *elf,
					Elf64_Shdr const *section)
{
  Elf64_Sym const	*symb_header;
  t_symbols		sym[section->sh_size / sizeof(Elf64_Sym)];
#endif
  int			i;

  symb_header = mapped + section->sh_offset + (i = 0);
  while ((void *)++symb_header < mapped + section->sh_offset + section->sh_size)
    if (ELF64_ST_TYPE(symb_header->st_info) != STT_FILE &&
	ELF64_ST_TYPE(symb_header->st_info) != STT_SECTION)
      {
	sym[i].name = GET_SYMB_NAME(elf, section, symb_header);
	sym[i].value = symb_header->st_value;
#ifdef ELFCLASS_32
	sym[i].type = get_type_32(elf, section, symb_header);
	sym[i++].print_val = PRINT_VAL_32(symb_header);
#elif defined(ELFCLASS_64)
	sym[i].type = get_type_64(elf, section, symb_header);
	sym[i++].print_val = PRINT_VAL_64(symb_header);
#endif
      }
  sort_symbs(sym, i);
  disp_symbs(sym, i);
}

#ifdef ELFCLASS_32
void			my_nm_32(void const *mapped, Elf32_Ehdr const *elf,
				 char const *filename)
{
  Elf32_Shdr const	*section;
#elif defined(ELFCLASS_64)
void			my_nm_64(void const *mapped, Elf64_Ehdr const *elf,
				 char const *filename)
{
  Elf64_Shdr const	*section;
#endif
  int		i;

  i = -1;
  while (++i < elf->e_shnum)
    {
      section = mapped + (elf->e_shoff + i * elf->e_shentsize);
      if (section->sh_type == SHT_SYMTAB)
#ifdef ELFCLASS_32
	return (parse_symtab_32(mapped, elf, section));
#elif defined(ELFCLASS_64)
	return (parse_symtab_64(mapped, elf, section));
#endif
    }
  fprintf(stderr, "%s: %s: no symbols\n",
	  program_invocation_short_name, filename);
}
