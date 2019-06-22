IDIR =../include
CC=gcc
CFLAGS= -std=c99 -I$(IDIR)

ODIR=../obj
LDIR =../lib

LIBS=-ledit

_DEPS = hello_make.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = hello_func.o repl.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS) | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

../bin/byolisp: $(OBJ) | bin
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR):
	mkdir -p $@

bin:
	mkdir -p ../$@

.PHONY: clean

clean:
	rm -rf ../bin $(ODIR) *~ core $(INCDIR)/*~ 