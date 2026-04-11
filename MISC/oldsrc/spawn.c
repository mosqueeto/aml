/*	spawn.c
 *
 *	Copyright (c) 1989 by Kent Crispin and Songbird Software
 *	All Rights reserved.
 *
 *	Substantial parts of this code came from the original 
 *	public domain MicroEmacs, by David G. Conroy
 *
 *	The routines in this file are called to create a subjob
 *	running a command interpreter. 
 *
 *History:
 */
 
#include	<stdio.h>
#include	"ed.h"

#include	<dos.h>


/*
 * Create a subjob with a copy
 * of the command intrepreter in it. When the
 * command interpreter exits, mark the screen as
 * garbage so that you do a full repaint. Bound
 * to "C-C".
 */
spawncli(f, n)
{
	movecursor(term.t_nrow, 0);		/* Seek to last line.	*/
	(*term.t_flush)();
	ttclose();
/*	execall("\\command.com", "");		 Run CLI.		*/
	ttopen();
	sgarbf = TRUE;
	return(TRUE);
}

/*
 * Run a one-liner in a subjob.
 * When the command returns, wait for a single
 * character to be typed, then mark the screen as
 * garbage so a full repaint is done.
 * Bound to "C-X !".
 */
spawn(f, n)
{
	register int	s;
	char		line[NLINE];
	if ((s=mlreply("!", line, NLINE)) != TRUE)
		return (s);
	ttclose();
	system(line);
	ttopen();
	mlputs("\n[End]");
	while ((*term.t_getchar)() != '\r')	/* Pause.		*/
		;
	sgarbf = TRUE;
	return (TRUE);
}
