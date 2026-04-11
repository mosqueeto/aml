#include <stdio.h>
#include <ctype.h>
#include <sys/errno.h>
#include <stdlib.h>
#include "midifile.h"

#define BUFZ 4096

FILE *aml_out;
FILE *fp;

int  trk_len_p;
long nbytes = 0;

int prev_event = 0;

void create_midi_file(name)
char *name;
{
    int i;
//  char header[] = {'M','T','h','d',0,0,0,6,0,0,0,1,0xe2,0x50,
    char header[] = {
        'M','T','h','d',    // chunk type
        0,0,0,6,            // length in bytes 32 bits
        0,0,                // format (0) 6 bits
        0,1,                // number of tracks (always 1 for format 0)
        //time division for delta times
//        2,0,  // This is 512 (0000 0010   0000 0000)
        1,244,  // 500 : 0000 0001  1111 0100
//        -25,40,  // high bit set -> smpte
        'M','T','r','k' };  // track beginning marker

    if( !(fp = fopen(name,"w")) ) {
        fprintf(stderr,"couldn't open output file <%s>\n",name);
        exit(1);
    }

    for( i=0;i<sizeof(header);i++ ) putc(header[i],fp);

    /* save where we will write the length when done */
    trk_len_p = i;

    /* save 4 bytes for it */
    for( i=0;i<4;i++ ) putc(' ',fp);
}

void mput(c)
int c;
{
    nbytes++;
    putc(c,fp);
// printf(" %02x",c);
}

void close_midi_file()
{
    union {
        long l;
        char c[4];
    } u;

    /* put "end of track" meta-event */
    mput(0x00,fp);
    mput(0xff,fp);
    mput(0x2f,fp);
    mput(0x00,fp);

    u.l = nbytes;
    
    /* go back and fill in track length */
    fseek(fp,(long)trk_len_p,SEEK_SET);
    putc(u.c[3],fp);
    putc(u.c[2],fp);
    putc(u.c[1],fp);
    putc(u.c[0],fp);
    fclose(fp);
}

void mput_delay(d)
int d;
{
    int i1,i2,i3;

//  printf("\n %06x",d);
    if( d < 128 ) {
        mput(d);
        return;
    }

    i3 = d & 0x7F;
    d >>= 7;
    i2 = ((d & 0x7F) | 0x80);

    if( d < 128 ) {
        mput(i2);
        mput(i3);
        return;
    }

    d >>= 7;
    i1 = ((d & 0x7F) | 0x80);
    mput(i1);
    mput(i2);
    mput(i3);
}

int write_note(delay,event,chan,note,vel)
int delay,event,chan,note,vel;
{
//  printf("\n%8d,%4d,%4d,%4d",delay,event,note,vel);
    mput_delay(delay);
    if( event != prev_event ) {
        prev_event = event;
        mput(event);
    }
    mput(note);
    mput(vel);
}
#if 0

void write_track()
{
    char data[2];
    int duration = 0;
    int delay;
    int e,n,v,c,d;

//    while( fscanf(aml_out,"%d,%d,%d,%d,%d",&e,&n,&v,&c,&d)==5 ) { 
      while( scanf("%d,%d,%d,%d,%d",&e,&n,&v,&c,&d)==5 ) { 
	delay = duration;
	duration = d;
        printf("%d %d %d %d %d\n",e,n,v,c,delay);
	if( e == NOTE_ON  ) write_note(delay,NOTE_ON ,c,n,v);
	if( e == NOTE_OFF ) write_note(delay,NOTE_OFF,c,n,v);
    }

    return;
}
main(argc,argv)
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

#endif
