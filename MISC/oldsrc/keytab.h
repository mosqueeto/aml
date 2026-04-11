/*	keytable.h
 *
 *	Copyright (c) 1989 by Kent Crispin and Songbird Software
 *	All Rights reserved.
 *
 *	Substantial parts of this code came from the original 
 *	public domain MicroEmacs, by David G. Conroy
 *
 *History:
 */
 
typedef	struct	{
	short	k_code;			/* Key code			*/
	int	(*k_fp)();		/* Routine to handle it		*/
}	KEYTAB;
extern	char	*strcpy();		/* copy string			*/

 /*
 * Command table.
 * This table  is *roughly* in ASCII
 * order, left to right across the characters
 * of the command. This expains the funny
 * location of the control-X commands.
 *
 *	Should be arranged in order of frequency of use and speed of
 *	execution (kc)
 */
KEYTAB	keytab[] = {
	CTRL|'A',		gotobol,
	CTRL|'B',		backchar,
	CTRL|'C',		cmpl_entity,
	CTRL|'D',		forwdel,
	CTRL|'E',		gotoeol,
	CTRL|'F',		forwchar,
	CTRL|'G',		ctrlg,
	CTRL|'H',		backdel,
	CTRL|'I',		tab,
	CTRL|'J',		indent,
	CTRL|'K',		kill,
	CTRL|'L',		refresh,
	CTRL|'M',		newline,
	CTRL|'N',		forwline,
/*	CTRL|'O',		openline, */
	CTRL|'P',		backline,
	CTRL|'Q',		quote,		/* Often unreachable	*/
	CTRL|'R',		backsearch,
	CTRL|'S',		forwsearch,	/* Often unreachable	*/
/*	CTRL|'T',		twiddle, */
	CTRL|'V',		forwpage,
	CTRL|'W',		killregion,
	CTRL|'Y',		yank,
	CTRL|'Z',		backpage,
	CTLX|CTRL|'B',		listbuffers,
	CTLX|CTRL|'C',		quit,		/* Hard quit.		*/
	CTLX|CTRL|'F',		filename,
/*	CTLX|CTRL|'L',		lowerregion, */
	CTLX|CTRL|'O',		deblank,
	CTLX|CTRL|'N',		mvdnwind,
	CTLX|CTRL|'P',		mvupwind,
	CTLX|CTRL|'Q',		quickexit,
	CTLX|CTRL|'R',		fileread,
	CTLX|CTRL|'S',		filesave,	/* Often unreachable	*/
/*	CTLX|CTRL|'U',		upperregion, */
	CTLX|CTRL|'V',		filevisit,
	CTLX|CTRL|'W',		filewrite,
	CTLX|CTRL|'X',		swapmark,
	CTLX|CTRL|'Z',		shrinkwind,
	CTLX|'!',		spawn,		/* Run 1 command.	*/
	CTLX|'=',		showcpos,
	CTLX|'(',		ctlxlp,
	CTLX|')',		ctlxrp,
	CTLX|'1',		onlywind,
	CTLX|'2',		splitwind,
	CTLX|'B',		usebuffer,
	CTLX|'E',		ctlxe,
	CTLX|'F',		setfillcol,
	CTLX|'K',		killbuffer,
	CTLX|'N',		nextwind,
	CTLX|'P',		prevwind,
	CTLX|'Z',		enlargewind,
	META|CTRL|'H',		delbword,
	META|' ',		setmark,
	META|'!',		reposition,
	META|'>',		gotoeob,
	META|'<',		gotobob,
	META|'B',		backword,
/*	META|'C',		capword, */
	META|'D',		delfword,
	META|'F',		forwword,
/*	META|'L',		lowerword,*/
	META|'Q',		quote,
	META|'R',		backsearch,
	META|'S',		forwsearch,
/*	META|'U',		upperword, */
	META|'V',		backpage,
	META|'W',		copyregion,
	META|0x7F,		delbword,
	0x7F,			backdel
};

#define	NKEYTAB	(sizeof(keytab)/sizeof(keytab[0]))
