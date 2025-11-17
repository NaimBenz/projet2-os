TOPDIR  := ./

SRCDIR_CLIENT  := $(TOPDIR)src/client/
OBJDIR_CLIENT  := $(TOPDIR)obj/client/
BINDIR_CLIENT  := $(TOPDIR)
NAME_CLIENT    := client
EXE_CLIENT     := $(BINDIR)$(NAME_CLIENT)

SRCDIR_SERVEUR  := $(TOPDIR)src/serveur/
OBJDIR_SERVEUR  := $(TOPDIR)obj/serveur/
BINDIR_SERVEUR  := $(TOPDIR)
NAME_SERVEUR    := serveur
EXE_SERVEUR     := $(BINDIR)$(NAME_SERVEUR)

SFILES  := c
OFILES  := o
CC      := gcc
CFLAGS  := -Wall -Wextra -O3 -std=gnu2x -pedantic -march=native -Wnull-dereference -Winline -Wconversion -pthread -flto -pipe -g -fsanitize=address,undefined
LIBS    := -fsanitize=address,undefined

SOURCES_CLIENT := $(shell find $(SRCDIR_CLIENT) -name "*.$(SFILES)")
OBJECTS_CLIENT := $(patsubst $(SRCDIR_CLIENT)%.$(SFILES), $(OBJDIR_CLIENT)%.$(OFILES), $(SOURCES_CLIENT))

SOURCES_SERVEUR := $(shell find $(SRCDIR_SERVEUR) -name "*.$(SFILES)")
OBJECTS_SERVEUR := $(patsubst $(SRCDIR_SERVEUR)%.$(SFILES), $(OBJDIR_SERVEUR)%.$(OFILES), $(SOURCES_SERVEUR))

SOURCES_COMMUN := $(shell find ./src/comm/ -name "*.$(SFILES)")
OBJECTS_COMMUN := $(patsubst ./src/comm/%.$(SFILES), ./obj/%.$(OFILES), $(SOURCES_COMMUN))


ALLFILES := $(SOURCES_CLIENT) $(SOURCES_SERVEUR) $(SOURCES_COMMUN)

.PHONY: all clean client serveur

all: $(EXE_CLIENT) $(EXE_SERVEUR)

$(EXE_CLIENT): $(OBJECTS_CLIENT) $(OBJECTS_COMMUN)
	$(CC) $^ -o $@ $(LIBS)

$(EXE_SERVEUR): $(OBJECTS_SERVEUR) $(OBJECTS_COMMUN)
	$(CC) $^ -o $@ $(LIBS)

$(OBJDIR_CLIENT)%$(OFILES): $(SRCDIR_CLIENT)%$(SFILES)
	$(CC) $(CFLAGS) $< -c -o $@

$(OBJDIR_SERVEUR)%$(OFILES): $(SRCDIR_SERVEUR)%$(SFILES)
	$(CC) $(CFLAGS) $< -c -o $@

./obj/%$(OFILES): ./src/comm/%$(SFILES)
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	@rm -f $(OBJDIR_CLIENT) $(OBJDIR_SERVEUR) $(EXE_CLIENT) $(EXE_SERVEUR)
