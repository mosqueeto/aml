/*
    atom -- aml output file to midi converter
        atom [-i aml_output] midifile

	default input file is "aml.out"
*/
    

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/errno.h>
#include "midifile.h"

FILE *aml_out;
FILE *fp;

void write_track()
{
    char data[2];
    int duration = 0;
    int delay;
    int e,n,v,c,d;

    while( fscanf(aml_out,"%d,%d,%d,%d,%d",&e,&n,&v,&c,&d)==5 ) { 
	delay = duration;
	duration = d;
//        printf("%d %d %d %d %d\n",e,n,v,c,delay);
	if( e == NOTE_ON ) write_note(delay,NOTE_ON ,c,n,v);
	if( e == NOTE_OFF ) write_note(delay,NOTE_OFF,c,n,v);
    }

    return;
}

int main(argc,argv)
int argc;
char **argv;
{
    if( !(aml_out = fopen("aml.out","r")) ) {
        fprintf(stderr,"couldn't open output file <%s>\n",argv[1]);
        exit(1);
    }

    create_midi_file(argv[1]);
    write_track();
    close_midi_file();
    
    exit(0);
}

