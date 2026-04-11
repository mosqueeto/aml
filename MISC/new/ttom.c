/*
    ttom -- text to midi
        ttom midifile <datafile
            or
        datasource | ttom midifile

        convert standard in to a midi representation of the data, and
        write it to a standard midi file.  The midi data is designed
        to so that the sound produced will permit decoding by a pitch
        to midi device.  Two things are done to increase the musical
	interest:  First, the notes are selected from a diatonic 
	scale, instead of a chromatic.  And second, the rhythm is 
        also varied algorithmically -- any decoding from the sound 
        should ignore all rhythmic variation.

	Code uses "midifilelib" from Tim Thompson & Michael 
	Czeiszperger, and is cobbled from one of their examples.

*/
    

#include <stdio.h>
#include <ctype.h>
#include "midifile.h"

#define ROOT 36
#define TICK 120

FILE *fp;

/* offsets for three octaves of diatonic major scale */
int scale[] = {0,2,4,5,7,9,11,12,14,16,17,19,23,24,26,28,29,31,33,35,36};

mputc(c) { return(putc(c,fp));}

int writetrack(track)
int track;
{
    int note_duration;
    int rest_duration;
    int high_nybble;
    int low_nybble;
    int tick = TICK;
    int beat;
    int vel;
    char c;
    char n1[2];
    char n2[2];

    mf_write_tempo((long)100000);
    tick = 60;
    
    vel = 64;
    beat = 0;
    while( (c = getchar()) != EOF ) {

        high_nybble = (c>>4) & 0xf;
        low_nybble  = c & 0xf;
        vel = 64;
        if( beat == 0 ) vel =96;
	
	/* low note */
        n1[0] = scale[low_nybble]+ROOT;	/* note number */
        n1[1] = vel;                    /* velocity */

	/* high note */
        n2[0] = scale[high_nybble]+n1[0]+12;
        n2[1] = vel;

	/* shouldn't happen */
	if( n1[0] >= n2[0] ) {
		printf("warning -- voice crossover! lo = %d; hi = %d\n",	
			n1[0],n2[0]);
	}

	/* note_duration needs to be long enough for pitch detectors */
	note_duration = tick*((c&15) + 4);
	if( isspace(c) ) {
		rest_duration = tick * 16;
	} 
	else {
		rest_duration = tick*(((c>>5)&3));
	}

        beat += (note_duration + rest_duration);
        if( beat > tick * 256 ) beat = 0;

        if(!mf_write_midi_event(rest_duration,note_on,1,n1,2)) 	return(-1);
        if(!mf_write_midi_event(0,note_on,1,n2,2))		return(-1);

        if(!mf_write_midi_event(note_duration,note_off,1,n1,2))	return(-1);
        if(!mf_write_midi_event(0,note_off,1,n2,2))		return(-1);
    }

    return(1);
} /* end of write_track() */

main(argc,argv)
char **argv;
{
    if( !(fp = fopen(argv[1],"w")) ) exit(1);

    Mf_putc = mputc;
    Mf_writetrack = writetrack;
    mfwrite(0,1,480,fp);
}

