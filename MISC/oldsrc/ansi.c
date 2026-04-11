/*	ansi.c
 *		-- ansi display driver
 *
 *	Copyright (c) 1989 by Kent Crispin and Songbird Software
 *	All Rights reserved.
 *
 *	Substantial parts of this code came from the original 
 *	public domain MicroEmacs, by David G. Conroy
 *
 *	The routines in this file provide support for ANSI style
 * 	terminals over a serial line.  The serial I/O services are
 *	provided by routines in "termio.c".  It compiles into
 *	nothing if not an ANSI device.
 *
 *History:
 */
 
#include	<stdio.h>
#include	"ed.h"

#if	ANSI

#define	NROW	24			/* Screen size.			*/
#define	NCOL	80			/* Edit if you want to.		*/
#define	BEL	0x07			/* BEL character.		*/
#define	ESC	0x1B			/* ESC character.		*/

extern	int	ttopen();		/* Forward references.		*/
extern	int	ttgetc();
extern	int	ttputc();
extern	int	ttflush();
extern	int	ttclose();
extern	int	ansimove();
extern	int	ansieeol();
extern	int	ansieeop();
extern	int	ansibeep();
extern	int	ansiopen();

/*
 * Standard terminal interface
 * dispatch table. Most of the fields
 * point into "termio" code.
 */
TERM	term	= {
	NROW-1,
	NCOL,
	ansiopen,
	ttclose,
	ttgetc,
	ttputc,
	ttflush,
	ansimove,
	ansieeol,
	ansieeop,
	ansibeep
};

ansimove(row, col)
{
	ttputc(ESC);
	ttputc('[');
	ansiparm(row+1);
	ttputc(';');
	ansiparm(col+1);
	ttputc('H');
}

ansieeol()
{
	ttputc(ESC);
	ttputc('[');
	ttputc('K');
}

ansieeop()
{
	ttputc(ESC);
	ttputc('[');
	ttputc('J');
}

ansibeep()
{
	ttputc(BEL);
	ttflush();
}

ansiparm(n)
register int	n;
{
	register int	q;

	q = n/10;
	if (q != 0)
		ansiparm(q);
	ttputc((n%10) + '0');
}

#endif

ansiopen()
{
	ttopen();
}
