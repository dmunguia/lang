IDIR =../include
CC=gcc
CFLAGS= -ggdb -std=c99 -I$(IDIR)

ODIR=../obj
LDIR =../lib

LIBS=-ledit -lm

_DEPS = mpc.h parser.h evaluator.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = mpc.o parser.o evaluator.o repl.o
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