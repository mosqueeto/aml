/* 	termio.c
 *
 *	Copyright (c) 1989 by Kent Crispin and Songbird Software
 *	All Rights reserved.
 *
 *	Substantial parts of this code came from the original 
 *	public domain MicroEmacs, by David G. Conroy
 *
 *	The functions in this file negotiate with the operating
 *	system for characters, and write characters in a barely
 *	buffered fashion on the display.  All operating systems. 
 *
 *History:
 */
#include	<stdio.h>
#include	"ed.h"

#include	<dos.h>

#define TYPAHEAD_MAX 32
char typahead_stk[TYPAHEAD_MAX];
int typahead_top = 0;

/*
 * This function is called once
 * to set up the terminal device streams.
 */
ttopen()
{
/*
 * Redefine cursor keys (as described in DOS Technical Reference Manual
 * p. 2-11, DOS BASIC Manual p. G-6) to mean what the user might expect.
 */
	static char *control[] = {
		"\033[0;72;16p",	/* up    = <ctrl-p>  */
		"\033[0;77;6p",		/* right = <ctrl-f>  */
		"\033[0;75;2p",		/* left  = <ctrl-b>  */
		"\033[0;80;14p",	/* down  = <ctrl-n>  */
		"\033[0;81;22p",	/* pg dn = <ctrl-v>  */
		"\033[0;73;3p",		/* pg up = <ctrl-c>  */
		"\033[0;71;96;60p",	/* home  = `<    */
		"\033[0;79;96;62p",	/* end   = `>    */
		"\033[0;83;127p",	/* del   = del       */
		"\033[0;3;96;46p"	/* <ctrl-@> = `. */
	};
	register char *cp;
	register int i;

	for (i = 0; i < sizeof(control)/sizeof(char *); i++) {
		for (cp = control[i]; *cp; )
			ttputc(*cp++);
	}
}

/*
 * This function gets called just
 * before we go back home to the command interpreter.
 */
ttclose()
{
/* Redefine cursor keys to default values. */
	static char *control[] = {
		"\033[0;72;0;72p",
		"\033[0;77;0;77p",
		"\033[0;75;0;75p",
		"\033[0;80;0;80p",
		"\033[0;81;0;81p",
		"\033[0;73;0;73p",
		"\033[0;71;0;71p",
		"\033[0;79;0;79p",
		"\033[0;83;0;83p",
		"\033[0;3;0;3p"
	};
	register char *cp;
	register int i;

	for (i = 0; i < sizeof(control)/sizeof(char *); i++) {
		for (cp = control[i]; *cp; )
			ttputc(*cp++);
	}
}

/*
 * Write a character to the display.
 */
ttputc(c)
{
/*	bdos(6, c, 0); */
	putch(c);
}

/*
 * Flush terminal buffer. Does real work
 * where the terminal output is buffered up. A
 * no-operation on systems where byte at a time
 * terminal I/O is done.
 */
ttflush()
{
}

/*
 * Read a character from the terminal,
 * performing no editing and doing no echo at all.
 */
ttgetc()
{
	int c;
	if( typahead_top > 0 ) 
		c = typahead_stk[--typahead_top];
	else c = (bdos(7, 0, 0) & 0xff);
	return c;
	return 
}
ttypahead()
{
	int c = -1;
	if( kbhit() ) {
		c = bdos(7, 0, 0) & 0xff;
		if( typahead_top < TYPAHEAD_MAX ) 
			typahead_stk[typahead_top++] = c;
	} 
	return c;
}
