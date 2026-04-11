CC = cc
CFLAGS=         -g
LIBS=

OFILES=	set.o entity.o io.o node.o param.o seq.o dur.o \
	func.o note.o util.o ml1.o play.o

CFILES=	set.c entity.c io.c node.c param.c seq.c dur.c \
	func.c note.c util.c

HFILES= ml1.h midi.h

ml1.o : ml1.h sequence.h

pl.o : ml1.h sequence.h

play.o : ml1.h sequence.h

ml1:	$(OFILES) $(HFILES)
	$(CC) $(CFLAGS) $(OFILES) $(LIBS) -o ml1

pl:	play.o pl.o util.o
	$(CC) $(CFLAGS) play.o pl.o util.o -o pl


