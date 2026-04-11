#$Id: Makefile,v 1.2 1997/10/31 14:08:14 kent Exp kent $
CC = cc
CFLAGS=         -g 
LIBS=
BASE= /home/kent/src/music
INCS= -I$(BASE)/include

OFILES=	midifile.o song.o seq.o set.o io.o node.o param.o fun.o \
	note.o util.o aml.o

CFILES=	midifile.c song.c seq.c set.c io.c node.c param.c fun.c  \
	note.c util.c aml.c

HFILES= aml.h

all:	aml atom

aml:	$(OFILES) $(HFILES)
	$(CC) $(CFLAGS) $(OFILES) $(LIBS) -o aml

atom:	atom.c midifile.o
	$(CC) $(CFLAGS) atom.c midifile.o -o atom

#pl:	play.o pl.o util.o
#	$(CC) $(CFLAGS) play.o pl.o util.o -o pl
#
install:	aml
	cp aml /usr/local/bin; chown bin:bin /usr/local/bin/aml; \
	chmod 755 /usr/local/bin/aml

clean:
	rm -f *.o *.out *.mid core ,,*

$(OFILES) : $(HFILES)

