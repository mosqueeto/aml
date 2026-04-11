/*
    atom -- aml output file to midi converter
        atom [-i aml_output] midifile

	default input file is "aml.out"

	Code uses "midifilelib" from Tim Thompson & Michael 
	Czeiszperger, and is cobbled from one of their examples.

*/
    

#include <stdio.h>
#include <ctype.h>
#include <sys/errno.h>
#include "midifile.h"

#define TICK 120

static char rcsid[] = "$Id$";

FILE *aml_out;
FILE *fp;

mputc(c) { return(putc(c,fp));}

int writetrack(track)
int track;
{
    char data[2];
    int duration = 0;
    int delay;
    int e,n,v,c,d;

    mf_write_tempo((long)1000000);

    
    while( fscanf(aml_out,"%d,%d,%d,%d,%d",&e,&n,&v,&c,&d)==5 ) { 

	delay = duration;
	duration = d;
//        printf("%d %d %d %d %d\n",e,n,v,c,delay);

        data[0] = n;
        data[1] = v;
	if( e == 1 ) {
            if(!mf_write_midi_event(delay,note_on,1,data,2)) {
                fprintf(stderr,"error writing note_on event, %d\n",errno); 	
		return(-1);
            }
        }
	if( e == 2 ) {
            if(!mf_write_midi_event(delay,note_off,1,data,2)) {
                fprintf(stderr,"error writing note_off event, %d\n",errno); 	
		return(-1);
            }
        }
    }

    return(1);
} /* end of write_track() */

main(argc,argv)
char **argv;
{
    if( !(fp = fopen(argv[1],"w")) ) {
        fprintf(stderr,"couldn't open output file <%s>\n",argv[1]);
        exit(1);
    }
    if( !(aml_out = fopen("aml.out","r")) ) {
        fprintf(stderr,"couldn't open output file <%s>\n",argv[1]);
        exit(1);
    }

    Mf_putc = mputc;
    Mf_writetrack = writetrack;
    mfwrite(0,1,480,fp);
    exit(0);
}

