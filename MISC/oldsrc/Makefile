#
#	Makefile for the various incarnations of AML.  To make aml, the
#	stand-alone compiler, type "make -Daml".  To make ami, the interactive
#	version, type "make -Dami".  To make the play routine, pl, type
#	"make -Dpl", and to make just a text editor, type "make -Ded".
#



!if $d(aml)
OBJECT=AML
$(OBJECT): set.obj    entity.obj  io.obj       node.obj \
	param.obj    seq.obj	 dur.obj      func.obj \
	aml.obj      note.obj    util.obj     play.obj

	tlink 	/co set entity io node param seq dur \
		func aml note util play,aml,nul; 

!elif $d(ami)
OBJECT=AMI
$(OBJECT): ansi.obj     basic.obj   buffer.obj   display.obj   file.obj \
	fileio.obj   line.obj    main.obj     random.obj    region.obj \
	search.obj   spawn.obj   termio.obj   window.obj    word.obj \
	set.obj    entity.obj  io.obj       node.obj      param.obj \
	seq.obj	     dur.obj     func.obj     note.obj      util.obj \
        play.obj 

	tcc -eami ansi basic buffer display file fileio line main random

!elif $d(pl)
OBJECT=PL
$(OBJECT): pl.x play.obj pl.obj util.obj 
	tcc pl play util
!elif $d(ed)
OBJECT=ED
$(OBJECT): ansi.obj  basic.obj buffer.obj display.obj file.obj fileio.obj \
	line.obj main.obj random.obj region.obj search.obj spawn.obj \
	termio.obj window.obj word.obj 

	tcc	-eed ansi basic buffer display file fileio line main \
		random region search spawn termio window word

!endif

.c.obj:
	tcc  -D$(OBJECT) -c $<

set.obj entity.obj io.obj node.obj param.obj seq.obj dur.obj \
func.obj note.obj util.obj : aml.h 

main.obj : aml.h ed.h keytab.h keyfun.h

aml.obj : aml.h

pl.obj : aml.h 

play.obj : aml.h

ansi.obj  basic.obj buffer.obj display.obj file.obj fileio.obj \
line.obj  random.obj region.obj search.obj spawn.obj \
termio.obj window.obj word.obj : ed.h

#	touch aml.c
	touch aml.c io.c util.c entity.c

pl.x:
	touch play.c
