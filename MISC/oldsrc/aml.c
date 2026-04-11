/*
 *	file: aml.c
 *		-- the main for the aml compiler
 *
 *	Copyright (c) 1989 by Kent Crispin and Songbird Software.
 *	All rights reserved.
 *
 */
 
/*
 * MAIN must be defined for "aml.h"
 */
 
#ifdef AML
#define MAIN 1
#define aml_main main
#endif

#include "aml.h"

char prompt[] = "%s [-T] [-o outfile] filename\n";

aml_main(argc,argv)
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
	    case 'T':
	    	trace = 1;
	    	break;
	    case 't':
	    	set_midi_trace(1);
	    	break;
	    default:
		error("illegal option: %s",*argv);
		break;
	    }
	}
	if( argc == 1 ) {
		init_io( *argv,ofname );
		compile(song_env);
		if( ofname == NULL ) {
			play(1);
		}
	}
	else {
		printf( prompt, pgmName );
	}
	exit(0);
}
