# Makefile for the ncurses game "NPONG"

# - Compiler - #
CC	= gcc

# - Directories - #
IDIR	= inc
ODIR	= obj
BDIR	= build

# - Flags - #
CFLAGS	= -Wall -Wextra -O2 -I $(IDIR)

# - Libraries - #
LIBS	= -lncurses

# - Files - #
_DEPS = npong.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o npong.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: files npong

$(ODIR)/%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

npong: $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)
	@echo "\n[SUCCESS]"

files:
	mkdir -p $(ODIR)
	mkdir -p build

.PHONY: clean

clean:
	rm -f obj/*.o
	@echo "\n[SUCCESS]"
