/*
** my_objdump_utils.c for  in /home/planch_j/rendu/c/PSU_2016_nmobjdump
** 
** Made by Jean PLANCHER
** Login   <planch_j@epitech.net>
** 
** Started on  Ven Feb 24 21:42:42 2017 Jean PLANCHER
** Last update Ven Feb 24 21:42:42 2017 Jean PLANCHER
*/

#include "nmobjdump.h"

void			print_section(void const *section, size_t size,
				      size_t line, char const *section_name)
{
  unsigned char const	*buffer;
  size_t		min;
  int			j;
  int			i;

  printf("Contents of section %s:\n", section_name);
  i = 0;
  while (size)
    {
      min = MIN(size, BUFF_SIZE);
      printf(" %0*lx", MAX(my_log8(size + line), 4), line);
      buffer = section + (BUFF_SIZE * i++);
      line += BUFF_SIZE;
      j = -1;
      while (++j < min)
	printf("%s%02x", (j % 4) ? "" : " ", buffer[j]);
      j = -1;
      while (j++ + min * 2.25 - 5 + (!(min / 4) || min % 4) < BUFF_SIZE * 2)
	printf(" ");
      j = -1;
      while (++j < BUFF_SIZE)
	printf("%c", j < min ? (!isprint(buffer[j]) ? '.' : buffer[j]) : ' ');
      printf("\n");
      size -= min;
    }
}

static void const	*error(char *filename)
{
  struct stat		st;
  void const		*mapped;
  int			fd;

  if ((fd = open(filename, O_RDONLY)) == -1 || stat(filename, &st) != 0)
    {
      fprintf(stderr, "%s: '%s': No such file\n",
	      program_invocation_short_name, filename);
      return (NULL);
    }
  if (S_ISDIR(st.st_mode))
    {
      fprintf(stderr, "%s: Warning: '%s' is not an ordinary file\n",
	      program_invocation_short_name, filename);
      return (NULL);
    }
  mapped = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  return (mapped);
}

static void		my_objdump(void const *mapped, char const *filename)
{
  t_ehdr	elf;

  elf.elf_64 = mapped;
  if (elf.elf_64->e_ident[EI_MAG0] != ELFMAG0 ||
      elf.elf_64->e_ident[EI_MAG1] != ELFMAG1 ||
      elf.elf_64->e_ident[EI_MAG2] != ELFMAG2 ||
      elf.elf_64->e_ident[EI_MAG3] != ELFMAG3)
    fprintf(stderr, "%s: %s: File format not recognized\n",
	    program_invocation_short_name, filename);
  else
    {
      if (elf.elf_32->e_ident[EI_CLASS] == ELFCLASS32)
	my_objdump_32(mapped, elf.elf_32, filename);
      else if (elf.elf_32->e_ident[EI_CLASS] == ELFCLASS64)
	my_objdump_64(mapped, elf.elf_64, filename);
    }
}

static void		parse_ar(void const *mapped, char const *filename)
{
  char const		*buff_name;
  struct ar_hdr const	*ar_header;
  struct stat		st;

  printf("In archive %s:\n", filename);
  stat(filename, &st);
  ar_header = mapped + SARMAG;
  ar_header = (void *)ar_header + atoi(ar_header->ar_size) +
    sizeof(*ar_header);
  buff_name = (void *)ar_header + sizeof(*ar_header);
  ar_header = (void *)ar_header + atoi(ar_header->ar_size) +
    sizeof(*ar_header);
  while ((void *)ar_header - mapped < st.st_size)
    {
      if (strchr(ar_header->ar_name, '/')[1] == ' ')
	my_objdump((void *)ar_header + sizeof(*ar_header),
		   strdup_to_char(ar_header->ar_name, '/'));
      else
	my_objdump((void *)ar_header + sizeof(*ar_header),
		   strdup_to_char(buff_name +
				  atoi(strchr(ar_header->ar_name, '/') +
				       1), '/'));
      ar_header = (void *)ar_header + atoi(ar_header->ar_size)
	+ sizeof(*ar_header);
    }
}

int		main(int ac, char *av[])
{
  void const	*mapped;
  int		i;

  if (ac == 1)
    return (1);
  i = 0;
  while (++i < ac)
    {
      mapped = error(av[i]);
      if (mapped == NULL)
	continue;
      if (mapped == MAP_FAILED)
	{
	  perror(av[i]);
	  continue;
	}
      if (strlen(mapped) > SARMAG && !strncmp(ARMAG, mapped, SARMAG))
        parse_ar(mapped, av[i]);
      else
	my_objdump(mapped, av[i]);
    }
  return (0);
}
