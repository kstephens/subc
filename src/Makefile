LINT=	gcc -c -o /dev/null -ansi -pedantic -Wall 

FILES=	cexpr.c cg.c decl.c error.c expr.c gen.c \
	main.c misc.c prep.c scan.c stmt.c sym.c
HDRS=	data.h decl.h defs.h prec.h

SCCDIR=	/u/scc
BINDIR=	/u/bin

all:	scc0 lib/crt0.o lib/libscc.a

test:	scc $(FILES)
	./scc -o scc2 $(FILES)
	cmp scc scc2 && rm -f scc2

install:	all
	./scc0 -o scc -D 'SCCDIR="$(SCCDIR)"' *.c
	cp scc $(BINDIR)
	cp include/* $(SCCDIR)/include
	cp lib/libscc.a lib/crt0.o $(SCCDIR)/lib

scc:	scc1 $(FILES)
	./scc1 -o scc $(FILES)

scc1:	scc0 lib/crt0.o lib/libscc.a $(FILES)
	./scc0 -o scc1 $(FILES)

scc0:	$(FILES)
	$(CC) -o scc0 $(FILES)

lib/libscc.a:
	make -f lib/Makefile

libc:
	make -f lib/Makefile

lib/crt0.o:	lib/crt0.s
	as -o lib/crt0.o lib/crt0.s

$(FILES):	$(HDRS)
gen.c:		cgen.h

lint:
	$(LINT) cexpr.c
	$(LINT) cg386.c
	$(LINT) cgx86_64.c
	$(LINT) decl.c
	$(LINT) error.c
	$(LINT) expr.c
	$(LINT) gen.c
	$(LINT) main.c
	$(LINT) misc.c
	$(LINT) prep.c
	$(LINT) scan.c
	$(LINT) stmt.c
	$(LINT) sym.c

clean:
	rm -f scc0 scc1 scc2 scc test.s *.o *.core core a.out lib/crt0.o
	make -f lib/Makefile clean
