CC	= gcc

RM	= rm -f

CFLAGS	+= -Wall -Iinclude -O3
LDFLAGS	+=

NMSRC	= $(addprefix src/, \
	my_nm.c)

NMUTIL	= $(addprefix src/, \
	my_nm_utils.c)

NMOBJ	= $(NMSRC:.c=.o32) $(NMSRC:.c=.o64) $(NMUTIL:.c=.o)

NMNAME	= my_nm

OBJSRC	= $(addprefix src/, \
	my_objdump.c)

OBJUTIL	= $(addprefix src/, \
	my_objdump_utils.c)

OBJOBJ	= $(OBJSRC:.c=.o32) $(OBJSRC:.c=.o64) $(OBJUTIL:.c=.o)

OBJNAME	= my_objdump


all: $(NMNAME) $(OBJNAME)

nm: $(NMNAME)

objdump: $(OBJNAME)

$(OBJNAME): $(OBJOBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(NMNAME): $(NMOBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(OBJOBJ)
	$(RM) $(NMOBJ)

fclean: clean
	$(RM) $(OBJNAME)
	$(RM) $(NMNAME)

re: fclean all

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

%.o32: %.c
	$(CC) -o $@ -c $< $(CFLAGS) -DELFCLASS_32

%.o64: %.c
	$(CC) -o $@ -c $< $(CFLAGS) -DELFCLASS_64

.PHONY: nm objdump clean fclean re
