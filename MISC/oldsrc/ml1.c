/*
 *	file: ml1.c
 *		-- the main for the ml1 compiler
 *
 *	Copyright (c) 1989 by Kent Crispin and Songbird Software.
 *	All rights reserved.
 *
 *Modification History:
 *890513:began merge with microemacs code (kc)
 *89.02.07 Hal - changed to support new data structures
 */
 
/*
 * MAIN must be defined for "ml1.h"
 */
 
#ifdef ML1
#define MAIN 1
#define ml1_main main
#endif

#include "ml1.h"

#ifdef NEWDS
#include "sequence.h"	/* Hal 89.02.12 */
#endif

#ifdef NEWDS
char prompt[] = "%s [-T] [-O sequencefile] [-o outfile] filename\n";
extern void setSongSequence( SongEventSequence * newSequence );
static	char *	sofname = NULL;	/* sequence output file name */
#else
char prompt[] = "%s [-T] [-o outfile] filename\n";
#endif

ml1_main(argc,argv)
int argc;
char *argv[];
{
	char *ofname = NULL;

	pgmName = argv[0];	/* remember name for error messages */
	while( (--argc > 0) && ((*++argv)[0] == '-') ) {
	   switch ((*argv)[1]) {
	    case 'o':
		if( argc ) {
		  ++argv; --argc;
		  ofname = *argv;}
                else
		  error("no output file name specified",NULL);
		break;
#ifdef NEWDS
	    case 'O':	/* This parameter supplies the filename for the 
			 * sequence to be written to.  This is mainly 
			 * used for debugging the sequence abstraction, 
			 * but it could eventually replace the "o" option.
			 */
		if( argc ) {
		  ++argv; --argc;
		  sofname = *argv;}
                else
		  error("no sequence output file name specified",NULL);
		break;
#endif
	    case 'T':
	    	trace = 1;
	    	break;
	    default:
		error("illegal option: %s",*argv);
		break;
	    }
	}
	if( argc == 1 ) {	/* Hal 89.02.12 */
		init_io( *argv,ofname );
#ifdef NEWDS
		/* Allocate a sequence and give it to the output routine. */
		SongEventSequence *theSequence;
		theSequence = newSequence( 0 );
		if( theSequence != 0 ) {
			setSongSequence( theSequence );
		} else {
			fprintf( stderr, "No sequence allocated, exiting.\n" );
			exit( 1 );
		}
		if( trace ) sequenceStatus( theSequence );
#endif
		compile(song_env);
		if( ofname == NULL ) {
#ifdef NEWDS
			if( trace ) sequenceStatus( theSequence );
			play( theSequence );
#else
			play(1);
#endif
		}
#ifdef NEWDS
		if( trace ) sequenceStatus( theSequence );
		/* Write the sequence to a file, if specified by user. */
		if( sofname != NULL ) {
			writeSequence( NULL, sofname, theSequence );
		} else if( trace ) {
			writeSequence( stdout, NULL, theSequence );
		}
		disposeSequence( theSequence );
#endif
	}
	else {
		printf( prompt, pgmName );	/* Hal 89.02.12 */
	}
	exit(0);
}
