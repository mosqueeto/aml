/*
 *	main.c
 *		-- main code for the integrated environment.
 *
 *	Copyright (c) 1989 by Kent Crispin and Songbird Software.
 *	All rights reserved.
 *
 *	Much of	this was taken from the public domain "MicroEMACS", by Dave
 *	G. Conroy, to whom I am deeply indebted. (kc)
 *
 */
 
#include	<stdio.h>
#include	<stdlib.h>
#define		MAIN	1
#include	"ed.h"
#include	"aml.h"
#include	"keyfun.h"
#include	"keytab.h"

#ifndef	GOOD
#define	GOOD	0
#endif

int	currow;				/* Working cursor row		*/
int	curcol;				/* Working cursor column	*/
int	fillcol;			/* Current fill column		*/
int	thisflag;			/* Flags, this command		*/
int	lastflag;			/* Flags, last command		*/
int	curgoal;			/* Goal column			*/
BUFFER	*curbp;				/* Current buffer		*/
WINDOW	*curwp;				/* Current window		*/
BUFFER	*bheadp;			/* BUFFER listhead		*/
WINDOW	*wheadp;			/* WINDOW listhead		*/
BUFFER	*blistp;			/* Buffer list BUFFER		*/
short	kbdm[NKBDM];			/* Macro			*/
short	*kbdmip;			/* Input  for above		*/
short	*kbdmop;			/* Output for above		*/
char	pat[NPAT];			/* Pattern			*/

char * strcpy();

main(argc, argv)
int  argc;
char *argv[];
{
	register int	c;
	register int	f;
	register int	n;
	register int	mflag;
	char		bname[NBUFN];

	for(n=0;n++;n<NKBDM) kbdm[n] = CTLX|')'; /* initialize this array */

	strcpy(bname, "main");			/* Work out the name of	*/
	if (argc > 1)				/* the default buffer.	*/
		makename(bname, argv[1]);
	edinit(bname);				/* Buffers, windows.	*/
	vtinit();				/* Displays.		*/
	if (argc > 1) {
		update();			/* You have to update	*/
		readin(argv[1]);		/* in case "[New file]"	*/
	}
	lastflag = 0;				/* Fake last flags.	*/
loop:
	update();				/* Fix up the screen	*/
	c = getkey();
	if (mpresf != FALSE) {
		mlerase();
		update();
		if (c == ' ')			/* ITS EMACS does this	*/
			goto loop;
	}
	f = FALSE;
	n = 1;
	if (c == (CTRL|'U')) {			/* ^U, start argument	*/
		f = TRUE;
		n = 4;				/* with argument of 4 */
		mflag = 0;			/* that can be discarded. */
		mlwrite("Arg: 4");
		while ((c=getkey()) >='0' && c<='9' || c==(CTRL|'U') || c=='-'){
			if (c == (CTRL|'U'))
				n = n*4;
			/*
			 * If dash, and start of argument string, set arg.
			 * to -1.  Otherwise, insert it.
			 */
			else if (c == '-') {
				if (mflag)
					break;
				n = 0;
				mflag = -1;
			}
			/*
			 * If first digit entered, replace previous argument
			 * with digit and set sign.  Otherwise, append to arg.
			 */
			else {
				if (!mflag) {
					n = 0;
					mflag = 1;
				}
				n = 10*n + c - '0';
			}
			mlwrite("Arg: %d", (mflag >=0) ? n : (n ? -n : -1));
		}
		/*
		 * Make arguments preceded by a minus sign negative and change
		 * the special argument "^U -" to an effective "^U -1".
		 */
		if (mflag == -1) {
			if (n == 0)
				n++;
			n = -n;
		}
	}
	if (c == (CTRL|'X'))			/* ^X is a prefix	*/
		c = CTLX | getctl();
	if (kbdmip != NULL) {			/* Save macro strokes.	*/
		if (c!=(CTLX|')') && kbdmip>&kbdm[NKBDM-6]) {
			ctrlg(FALSE, 0);
			goto loop;
		}
		if (f != FALSE) {
			*kbdmip++ = (CTRL|'U');
			*kbdmip++ = n;
		}
		*kbdmip++ = c;
	}
	execute(c, f, n);			/* Do it.		*/
	goto loop;
}

/*
 * Initialize all of the buffers
 * and windows. The buffer name is passed down as
 * an argument, because the main routine may have been
 * told to read in a file by default, and we want the
 * buffer name to be right.
 */
edinit(bname)
char	bname[];
{
	register BUFFER	*bp;
	register WINDOW	*wp;

	bp = bfind(bname, TRUE, 0);		/* First buffer		*/
	blistp = bfind("[List]", TRUE, BFTEMP);	/* Buffer list buffer	*/
	wp = (WINDOW *) malloc(sizeof(WINDOW));	/* First window		*/
	if (bp==NULL || wp==NULL || blistp==NULL)
		abort();
	curbp  = bp;				/* Make this current	*/
	wheadp = wp;
	curwp  = wp;
	wp->w_wndp  = NULL;			/* Initialize window	*/
	wp->w_bufp  = bp;
	bp->b_nwnd  = 1;			/* Displayed.		*/
	wp->w_linep = bp->b_linep;
	wp->w_dotp  = bp->b_linep;
	wp->w_doto  = 0;
	wp->w_markp = NULL;
	wp->w_marko = 0;
	wp->w_toprow = 0;
	wp->w_ntrows = term.t_nrow-1;		/* "-1" for mode line.	*/
	wp->w_force = 0;
	wp->w_flag  = WFMODE|WFHARD;		/* Full.		*/
}
	
/*
 * This is the general command execution
 * routine. It handles the fake binding of all the
 * keys to "self-insert". It also clears out the "thisflag"
 * word, and arranges to move it to the "lastflag", so that
 * the next command can look at it. Return the status of
 * command.
 */
execute(c, f, n)
{
	register KEYTAB *ktp;
	register int	status;

	ktp = &keytab[0];			/* Look in key table.	*/
	while (ktp < &keytab[NKEYTAB]) {
		if (ktp->k_code == c) {
			thisflag = 0;
			status   = (*ktp->k_fp)(f, n);
			lastflag = thisflag;
			return (status);
		}
		++ktp;
	}

	/*
	 * If a space was typed, fill column is defined, the argument is non-
	 * negative, and we are now past fill column, perform word wrap.
	 */
	if (c == ' ' && fillcol > 0 && n>=0 && getccol(FALSE) > fillcol)
		wrapword();

	if ((c>=0x20 && c<=0x7E)		/* Self inserting.	*/
	||  (c>=0xA0 && c<=0xFE)) {
		if (n <= 0) {			/* Fenceposts.		*/
			lastflag = 0;
			return (n<0 ? FALSE : TRUE);
		}
		thisflag = 0;			/* For the future.	*/
		status   = linsert(n, c);
		lastflag = thisflag;
		return (status);
	}
	lastflag = 0;				/* Fake last flags.	*/
	return (FALSE);
}

/*
 * Read in a key.
 * Do the standard keyboard preprocessing.
 * Convert the keys to the internal character set.
 */
getkey()
{
	register int	c;
	c = (*term.t_getchar)();
	if (c == AGRAVE) {			/* Alternate M- prefix.	*/
		c = getctl();
		return (META | c);
	}
	
	if (c == METACH) {			/* Apply M- prefix	*/
		c = getctl();
		return (META | c);
	}

	if (c == CTRLCH) {			/* Apply C- prefix	*/
		c = getctl();
		return (CTRL | c);
	}
	if (c == CTMECH) {			/* Apply C-M- prefix	*/
		c = getctl();
		return (CTRL | META | c);
	}
	if (c>=0x00 && c<=0x1F)			/* C0 control -> C-	*/
		c = CTRL | (c+'@');
	return (c);
}

/*
 * Get a key.
 * Apply control modifications
 * to the read key.
 */
getctl()
{
	register int	c;

	c = (*term.t_getchar)();
	if (c>='a' && c<='z')			/* Force to upper	*/
		c -= 0x20;
	if (c>=0x00 && c<=0x1F)			/* C0 control -> C-	*/
		c = CTRL | (c+'@');
	return (c);
}

/*
 * Fancy quit command.
 * If the current buffer has changed
 * do a write current buffer and exit emacs,
 * otherwise simply exit.
 */
quickexit(f, n)
{
	if ((curbp->b_flag&BFCHG) != 0		/* Changed.		*/
	&& (curbp->b_flag&BFTEMP) == 0)		/* Real.		*/
		filesave(f, n);
	quit(f, n);				/* conditionally quit	*/
}

/*
 * Quit command. If an argument, always
 * quit. Otherwise confirm if a buffer has been
 * changed and not written out. Normally bound
 * to "C-X C-C".
 */
quit(f, n)
{
	register int	s;

	if (f != FALSE				/* Argument forces it.	*/
	|| anycb() == FALSE			/* All buffers clean.	*/
	|| (s=mlyesno("Quit")) == TRUE) {	/* User says it's OK.	*/
		vttidy();
		exit(GOOD);
	}
	return (s);
}

/*
 * Begin a keyboard macro.
 * Error if not at the top level
 * in keyboard processing. Set up
 * variables and return.
 */
ctlxlp(f, n)
{
	if (kbdmip!=NULL || kbdmop!=NULL) {
		mlwrite("Not now");
		return (FALSE);
	}
	mlwrite("[Start macro]");
	kbdmip = &kbdm[0];
	return (TRUE);
}

/*
 * End keyboard macro. Check for
 * the same limit conditions as the
 * above routine. Set up the variables
 * and return to the caller.
 */
ctlxrp(f, n)
{
	if (kbdmip == NULL) {
		mlwrite("Not now");
		return (FALSE);
	}
	mlwrite("[End macro]");
	kbdmip = NULL;
	return (TRUE);
}

/*
 * Execute a macro.
 * The command argument is the
 * number of times to loop. Quit as
 * soon as a command gets an error.
 * Return TRUE if all ok, else
 * FALSE.
 */
ctlxe(f, n)
{
	register int	c;
	register int	af;
	register int	an;
	register int	s;

	if (kbdmip!=NULL || kbdmop!=NULL) {
		mlwrite("Not now");
		return (FALSE);
	}
	if (n <= 0) 
		return (TRUE);
	do {
		kbdmop = &kbdm[0];
		do {
			af = FALSE;
			an = 1;
			if ((c = *kbdmop++) == (CTRL|'U')) {
				af = TRUE;
				an = *kbdmop++;
				c  = *kbdmop++;
			}
			s = TRUE;
		} while (c!=(CTLX|')') && (s=execute(c, af, an))==TRUE);
		kbdmop = NULL;
	} while (s==TRUE && --n);
	return (s);
}

/*
 * Abort.
 * Beep the beeper.
 * Kill off any keyboard macro,
 * etc., that is in progress.
 * Sometimes called as a routine,
 * to do general aborting of
 * stuff.
 */
ctrlg(f, n)
{
	(*term.t_beep)();
	if (kbdmip != NULL) {
		kbdm[0] = (CTLX|')');
		kbdmip  = NULL;
	}
	return (ABORT);
}
cmpl_entity(f,n)
{
	init_io(NULL,NULL);
	if( !compile(song_env) ) return FALSE;
	else play(n);
	return TRUE;
}
