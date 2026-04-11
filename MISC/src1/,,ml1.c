/*
 *	file: ml1.c
 *		-- the main for the ml1 compiler
 *
 *	Copyright (c) 1989 by Kent Crispin and Songbird Software.
 *	All rights reserved.
 *
 *Modification History:
 *890513:began merge with microemacs code (kc)
 */
 
/*
 * MAIN must be defined for "ml1.h"
 */
 
#ifdef ML1
#define MAIN 1
#define ml1_main main
#endif

#include "ml1.h"

char prompt[] = "%s [-T] [-o outfile] filename\n";

ml1_main(argc,argv)
int argc;
char *argv[];
{
	char *ofname = NULL;

	program = argv[0];	/* remember name for error messages */
	while( (--argc > 0) && ((*++argv)[0] == '-') ) {
	   switch ((*argv)[1]) {
	    case 'o':
		if( argc ) {
		  ++argv; --argc;
		  ofname = *argv;}
                else
		  error("no output file name specified",NULL);
		break;
	    case 'T':
	    	trace = 1;
	    	break;
	    default:
		error("illegal option: %s",*argv);
		break;
	    }
	}
	if( argc == 1 ) {
		init_io( *argv,ofname );
		compile(song_env);
//		if( ofname == NULL ) {
//			play(1);
//		}
	}
	else {
		printf( prompt, program );
	}
	exit(0);
}
