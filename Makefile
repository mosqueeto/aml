#$Id: Makefile,v 1.2 1997/10/31 14:08:14 kent Exp kent $
CC = cc
CFLAGS=         -g 
LIBS=
BASE= /home/kent/db/z/src/aml
INCS= -I$(BASE)/include
INST= /usr/local/bin



all:	
	cd src; make all


clean:
	cd src; make clean

tar:	clean
	tar cf aml.tar copyright samples tests Makefile bach doc src ; \
	cp aml.tar distrib	

#	cp aml.tar distrib/aml.tar.`date +%Y%m%d%H%M`; \ 

