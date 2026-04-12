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

#define AML
 
#ifdef AML
#define MAIN 1
#endif
#include "aml.h"

char prompt[] = "%s [-T] [-o outfile] filename\n";

int main(int argc, char *argv[])
{
	char *ofname = NULL;
        char buf[100];

	int perserve_output = 0;

	program = argv[0];	/* remember name for error messages */
	while( (--argc > 0) && ((*++argv)[0] == '-') ) {
	   switch ((*argv)[1]) {
	    case 'o':
		if( argc ) {
		  ++argv; --argc;
		  ofname = *argv;
                } else {
		  error("no output file name specified\n",NULL);
                }
		break;
	    case 't':
	    	trace = 1;
	    	break;
	    case 'P':
	    	print_object = 1;
	    	break;
	    default:
		error("illegal option: %s\n",*argv);
		break;
	    }
	}

	if( argc == 1 ) {
                if( !ofname || !strcmp( ofname,*argv ) ) {
                    sprintf(buf,"%s.mid",*argv);
                    ofname = buf;
                }
                    
		init_io( *argv,ofname );
		song(song_env);
		close_io();
	}
	else {
		printf( prompt, program );
	}
	exit(0);
}

