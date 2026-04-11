/*	keyfuncs.h
 *
 *	Copyright (c) 1989 by Kent Crispin and Songbird Software
 *	All Rights reserved.
 *
 *	Substantial parts of this code came from the original 
 *	public domain MicroEmacs, by David G. Conroy
 *
 *History:
 */

extern	int	backchar();		/* Move backward by characters	*/
extern	int	backdel();		/* Backward delete		*/
extern	int	backline();		/* Move backward by lines	*/
extern	int	backpage();		/* Move backward by pages	*/
extern	int	backsearch();		/* Search backwards		*/
extern	int	backword();		/* Backup by words		*/
extern	int	capword();		/* Initial capitalize word.	*/
extern	int	cmpl_entity();		/* Compile entity		*/
extern	int	copyregion();		/* Copy region to kill buffer.	*/
extern	int	ctrlg();		/* Abort out of things		*/
extern	int	ctlxe();		/* Execute macro		*/
extern	int	ctlxlp();		/* Begin macro			*/
extern	int	ctlxrp();		/* End macro			*/
extern	int	deblank();		/* Delete blank lines		*/
extern	int	delfword();		/* Delete forward word.		*/
extern	int	delbword();		/* Delete backward word.	*/
extern	int	enlargewind();		/* Enlarge display window.	*/
extern	int	fileread();		/* Get a file, read only	*/
extern	int	filevisit();		/* Get a file, read write	*/
extern	int	filewrite();		/* Write a file			*/
extern	int	filesave();		/* Save current file		*/
extern	int	filename();		/* Adjust file name		*/
extern	int	forwchar();		/* Move forward by characters	*/
extern	int	forwdel();		/* Forward delete		*/
extern	int	forwline();		/* Move forward by lines	*/
extern	int	forwpage();		/* Move forward by pages	*/
extern	int	forwsearch();		/* Search forward		*/
extern	int	forwword();		/* Advance by words		*/
extern	int	getccol();		/* Get current column		*/
extern	int	gotobob();		/* Move to start of buffer	*/
extern	int	gotobol();		/* Move to start of line	*/
extern	int	gotoeol();		/* Move to end of line		*/
extern	int	gotoeob();		/* Move to end of buffer	*/
extern	int	indent();		/* Insert CR-LF, then indent	*/
extern	int	kill();			/* Kill forward			*/
extern	int	killbuffer();		/* Make a buffer go away.	*/
extern	int	killregion();		/* Kill region.			*/
extern	int	listbuffers();		/* Display list of buffers	*/
extern	int	lowerregion();		/* Lower case region.		*/
extern	int	lowerword();		/* Lower case word.		*/
extern	int	mvdnwind();		/* Move window down		*/
extern	int	mvupwind();		/* Move window up		*/
extern	int	newline();		/* Insert CR-LF			*/
extern	int	nextwind();		/* Move to the next window	*/
extern	int	onlywind();		/* Make current window only one	*/
extern	int	openline();		/* Open up a blank line		*/
extern  int	prevwind();		/* Move to the previous window	*/
extern	int	quickexit();		/* Quick save and exit		*/
extern	int	quit();			/* Quit				*/
extern	int	quote();		/* Insert literal		*/
extern	int	refresh();		/* Refresh the screen		*/
extern	int	reposition();		/* Reposition window		*/
extern	int	setfillcol();		/* Set fill column.		*/
extern	int	setmark();		/* Set mark			*/
extern	int	showcpos();		/* Show the cursor position	*/
extern	int	spawn();		/* Run a command in a subjob.	*/
extern	int	splitwind();		/* Split current window		*/
extern	int	shrinkwind();		/* Shrink window.		*/
extern	int	swapmark();		/* Swap "." and mark		*/
extern	int	twiddle();		/* Twiddle characters		*/
extern	int	tab();			/* Insert tab			*/
extern	int	usebuffer();		/* Switch a window to a buffer	*/
extern	int	upperregion();		/* Upper case region.		*/
extern	int	upperword();		/* Upper case word.		*/
extern	int	yank();			/* Yank back from killbuffer.	*/

