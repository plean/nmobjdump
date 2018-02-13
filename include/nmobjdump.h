/*
** nmobjdump.h for  in /home/planch_j/rendu/c/PSU_2016_nmobjdump
** 
** Made by Jean PLANCHER
** Login   <planch_j@epitech.net>
** 
** Started on  Ven Feb 24 21:42:42 2017 Jean PLANCHER
** Last update Ven Feb 24 21:42:42 2017 Jean PLANCHER
*/

#ifndef OBJDUMP_H_
# define OBJDUMP_H_

# define _GNU_SOURCE

# include <sys/mman.h>
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>

# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <elf.h>
# include <ar.h>

# define MIN(x, y) (y ^ ((x ^ y) & -(x < y)))
# define MAX(x, y) (x ^ ((x ^ y) & -(x < y)))

# define MCHN_64(x) x == EM_X86_64 ? "i386:x86-64" : "Unknown"
# define MCHN_32(x) x == EM_386 ? "i386" : "Unknown"

# define PRINT_VAL_32(x) \
  (x->st_shndx == SHN_UNDEF || \
   (ELF32_ST_BIND(x->st_info) == STB_WEAK && \
    !x->st_value && ELF32_ST_TYPE(x->st_info) != STT_OBJECT))
# define PRINT_VAL_64(x) \
  (x->st_shndx == SHN_UNDEF || \
   (ELF64_ST_BIND(x->st_info) == STB_WEAK && \
    !x->st_value && ELF64_ST_TYPE(x->st_info) != STT_OBJECT))

/*
** Such norm
** Much bravo !
*/

# define GET_SYMB_NAME(elf, section, sym) (void *)elf + \
  ((typeof(section))((void *)elf + (elf->e_shoff + section->sh_link * \
				    elf->e_shentsize)))->sh_offset + \
  sym->st_name
# define GET_SECTION(elf, section, x)\
  ((typeof(section))((void *)elf + (x * elf->e_shentsize + elf->e_shoff)))
# define IS_FROM_SECTION(str) \
  !strcmp((void *)elf + GET_SECTION(elf, section, elf->e_shstrndx)->sh_offset \
	  + GET_SECTION(elf, section, sym->st_shndx)->sh_name, \
	  str)

static inline unsigned int	my_log2(size_t nb)
{
  unsigned int			r;

  r = 0;
  while (nb >>= 1)
    r++;
  return (r);
}

static inline unsigned int	my_log8(size_t nb)
{
  return (my_log2(nb) / 4 + 1);
}

static inline char	*strdup_to_char(char const *str, char del)
{
  return (strndup(str, strchr(str, del) - str));
}

static inline char	*remove_char_from_string(char const *s, char t)
{
  char			*str;
  int			i;
  int			j;

  i = 0;
  str = strdup(s);
  while (i < strlen(str))
    {
      if (str[i] == t)
	{
	  j = i;
	  while (j < strlen(str))
	    {
	      str[j] = str[j + 1];
	      j++;
	    }
	}
      else
	i++;
    }
  return (str);
}

# define BUFF_SIZE 16

typedef union		u_ehdr
{
  Elf64_Ehdr const	*elf_64;
  Elf32_Ehdr const	*elf_32;
}			t_ehdr;

typedef struct  s_symbols
{
  unsigned int  value;
  char          type;
  char const    *name;
  int		print_val;
}               t_symbols;

/*
** Objdump
*/

void	print_section(void const *section, size_t size, size_t line,
		      char const *section_name);
void	my_objdump_32_disp_header(void const *mapped, Elf32_Ehdr const *elf,
				  char const *filename);
void	my_objdump_64_disp_header(void const *mapped, Elf64_Ehdr const *elf,
				  char const *filename);
void	my_objdump_32(void const *mapped, Elf32_Ehdr const *elf,
		      char const *filename);
void	my_objdump_64(void const *mapped, Elf64_Ehdr const *elf,
		      char const *filename);

/*
** Nm
*/

void	disp_symbs(t_symbols const *sym, int i);
void	sort_symbs(t_symbols *sym, int i);
void	my_nm_64(void const *mapped, Elf64_Ehdr const *elf,
		 char const *filename);
void	my_nm_32(void const *mapped, Elf32_Ehdr const *elf,
		 char const *filename);

#endif /* OBJDUMP_H_ */
