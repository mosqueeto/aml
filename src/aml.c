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

#ifndef AML_DEFAULT_PLAYER
#define AML_DEFAULT_PLAYER "fluidsynth -i"
#endif

char prompt[] = "%s [-t] [-P] [-w] [-p] [-o outfile] filename\n";

int main(int argc, char *argv[])
{
	char *ofname = NULL;
	char buf[256];
	int  play = 0;

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
	    case 'w':
	    	nowarn = 1;
	    	break;
	    case 'p':
	    	play = 1;
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

		if( play ) {
			char cmd[512];
			const char *player = getenv("AML_PLAYER");
			if( !player ) player = AML_DEFAULT_PLAYER;
			snprintf(cmd, sizeof(cmd), "%s %s", player, ofname);
			system(cmd);
		}
	}
	else {
		printf( prompt, program );
	}
	exit(0);
}

