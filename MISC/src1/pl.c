/*	pl.c	-- stand-alone player of aml files
 *
 */

#define MAIN 1
#include "aml.h"
#include <stdlib.h>
#include <ctype.h>

FILE *sng_file;
int debug = 0;

main(argc,argv)
int argc;
char *argv[];
{
	char *fname = NULL;
	int e,n,v,c;
	int d;
	int rpt = 1;
	int mpu_dlay;
	int i = 0;
	char * rp = NULL;
	
	program = argv[0];	/* remember for use in error reporting */
	while( (--argc > 0) && ((*++argv)[0] == '-') ) {
	   switch ((*argv)[1]) {
	    case 'i':
		if( argc ) {
		  ++argv; --argc;
		  fname = *argv; }
                else
		  error("no input file name specified",NULL);
		break;
	    case 'D':
	    	debugflag = 1;
	    	break;
	    case 'T':	/* Hal 89.02.25 */
	    	trace = 1;
	    	break;
	    case 't':
	    	set_midi_trace(1);
	    	break;
	    case 'd':	/* delay */
		if( argc ) {
		  ++argv; --argc;
	    	  mpu_dlay = 0;
		  rp = *argv;
		  while( isdigit(*rp) ) mpu_dlay = mpu_dlay*10 + (*rp++ - '0'); 
		  set_mpu_delay(mpu_dlay);
		}
	        else error("no delay count specified",NULL);
		break;	    	
	    case 'r':	/* repeat count */
		if( argc ) {
		  ++argv; --argc;
	    	  rpt = 0;
		  rp = *argv;
		  while( isdigit(*rp) ) rpt = rpt*10 + (*rp++ - '0'); 
		}
                else error("no repeat count specified",NULL);
		break;	    	
	    default:
		error("illegal option: %s\n",*argv);
		break;
	    }
	}
	if( (argc < 1) && (fname == NULL) ) {
		if( (sng_file = fopen( "ml1.o","r" )) == NULL ) 
			error("couldn't open ml1.o\n",NULL);
	} else if( fname != NULL ) {
		if( (sng_file = fopen( fname,"r" )) == NULL )
			error("couldn't open input file %s\n",fname);
 	} else if( argc == 1 ) {
		if( (sng_file = fopen( *argv,"r" )) == NULL )
			error("couldn't open input file %s\n",*argv);
	} else {
		error("extra token on line: %s\n",*argv);
	}
	event = (byte*) malloc( 5000*sizeof(byte) );
	note  = (byte*) malloc( 5000*sizeof(byte) );
	vel   = (byte*) malloc( 5000*sizeof(byte) );
	chan  = (byte*) malloc( 5000*sizeof(byte) );
	dlay  = (int *) malloc( 5000*sizeof( int) );
 	output_index = 0;
 	
	while( fscanf(sng_file,"%d,%d,%d,%d,%d",&e,&n,&v,&c,&d)==5 ) {

		event[output_index] = e;
		note[output_index] = n & 0xff;
		vel[output_index] = v & 0xff;
		chan[output_index] = c & 0xf;
		delay[output_index] = d;
/*		printf("%4d,%4d,%4d,%4d,%8d\n",e,n,v,c,d);*/
		output_index++;

	}

	play(rpt);
	fclose(sng_file);

}


#if 0
void error(char *s1,char *s2 )
{
	printf(s1,s2);
	exit(1);
}
#endif
