/*	play.c -- midi interface for aml
 *
 *	copyright (c) 1989 by Songbird Software and Kent Crispin
 *			All rights reserved.
 */
#define TURBOC
#include "aml.h"
#include <StdLib.h>
#include <dos.h>
#include <time.h>

#define TRUE	1
#define FALSE	0
#define ERROR	-1

#define CTRL_G	0x07

#ifndef	MIDI_MAC
#ifndef MIDI_NONE
#ifndef MIDI_MPU
#define MIDI_MPU
#endif
#endif
#endif

/*	
	MPU-401 codes.  Note that ACK is the same as the MIDI 
	"active sensing" byte, of which we receive many.
*/
	  
#define	DATAPORT	0x330
#define	STATPORT	0x331
#define	COMPORT		0x331
#define	DSR_MASK	0x080
#define	DRR_MASK	0x040
#define	STAT_MSK	0x0C0
#define	ACK		0x0FE
#define	RST		0x0FF
#define	DISM		0x08B
#define	WTSD		0x0D0
#define	UART		0x03F
#define	MPU_RETRIES	0x080

/*	MIDI codes	*/
	  
#define	KEY_OFF	0x080
#define	KEY_ON		0x090
#define	POLY_KP_AT	0x0A0
#define	CTRL_CH		0x0B0
#define	PATCH_CH	0x0C0
#define	CHNL_KP_AT	0x0D0
#define WHEEL_CH	0x0E0
#define SYSTEM		0x0F0
#define ALL_NOTES_OFF	0x07B

typedef struct {
	int top;
	unsigned char data[512];
} stack;

stack input_stack;
void push( int b, stack *stk );
int empty( stack *stk );
int pop( stack *stk );

char channel_used[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char hexdigits[] = "0123456789ABCDEF";
#ifdef MSC 
struct dostime_t time0,time1;	/* used for timing, comes from dos.h */
#endif
#ifdef TURBOC
clock_t time0,time1;
#endif

/* the pause_constant will be refined by the "adjust_pause" call */
int pause_constant = 200;	/* a generous first guess */
int mpu_delay = 10;
int midi_trace = 0;		/* set to 1 if we want a trace */
jmp_buf play_done;		/* long jump buffer, in case of error */

void pause( int n );
int  send_midi_cmd( int command );
int  set_patch( int patch );
int  send_key_on( int k, int v, int c );
int  send_key_off( int k, int v, int c );
int  init_midi();
int  exit_midi();

int play(int repeats)
{
	int i = 0;
	int stat = 0;
	
	mlwrite("Beginning play...");
	
	if( setjmp(play_done) != 0 ) {
		stat = ERROR;
		goto done;
	}

	i = adjust_pause(1000);
		
	if( init_midi() == ERROR ) {
		stat = ERROR;
		goto done;
	}
	mlwrite("Playing...");
	for( repeats=repeats; repeats>0; repeats-- ) {
	    for( i=0; i<output_index; i++) {
		if( debugflag ) printf("%4d, %4d, %4d, %4d, %8d\n",
			event[i],note[i],vel[i],chan[i],dlay[i]);
		pause(dlay[i]);
		if( event[i] == NOTE_ON ) {
			channel_used[chan[i]] = 1;
			if( send_key_on(note[i],vel[i],chan[i]) == ERROR )
				longjmp(play_done,ERROR);
		} else
		if( event[i] == NOTE_OFF ) {
			channel_used[chan[i]] = 1;
			if( send_key_on(note[i],0,chan[i]) == ERROR )
				longjmp(play_done,ERROR);
		} else {
		}
	    }
	}
done:
	/* each of these arrays is deallocated after a run */
	free(event);
	free(note);
	free(vel);
	free(chan);
	free(dlay);

	exit_midi();
	if( stat == ERROR ) mlwrite("MIDI error.");
	else mlwrite("Done.");
}


char *midi_ntoa(  int b )
{
	char *tp;
	if     ( b==ACK )		tp = "ACK";
	else if( b==RST	) 		tp = "RST";
	else if( b==DISM ) 		tp = "DISM";
	else if( b==WTSD ) 		tp = "WTSD";
	else if( b==UART ) 		tp = "UART";
	else if( (b & 0xf0)==KEY_ON )	tp = "KEY_ON";
	else if( (b & 0xf0)==KEY_OFF )	tp = "KEY_OFF";
	else if( b==PATCH_CH ) 		tp = "PATCH_CH";
	else if( b==ERROR )		tp = "ERROR";
	else {  
		tp = " <??>";
		tp[2] = hexdigits[ ((b>>4) & 0xf) ];
		tp[3] = hexdigits[ (b & 0xf) ];
	}
	return( tp );
}

void set_mpu_delay(int d) { mpu_delay = d; }
void do_mpu_delay()
{
	int i;
	for( i=0; i<mpu_delay ; i++ );
}

int get_midi_data()
{
	int stat = 0;
	int data = ERROR;
	int i = MPU_RETRIES;
	if( debugflag ) printf("get_midi_data");
	if( ! empty(&input_stack) ) return pop(&input_stack);
	while( i-- ) {
		stat = inp(STATPORT);
		if( ! (stat & DSR_MASK) ) {
			data = inp(DATAPORT);
			break;
		}
	}		
	if( debugflag ) printf(", data = %s\n",midi_ntoa( data) );
	return( data );
}

int put_midi_data( int data )
{
	int stat = 0;
	int indata = 0;
	int result = ERROR;

	if(debugflag) printf("put_midi_data: data = %s",midi_ntoa(data));
	while( TRUE ) {
		stat = inp(STATPORT);
		if( ! (stat & DSR_MASK) ){
			indata = inp(DATAPORT);
			if( midi_trace ) printf("got %s\n",midi_ntoa(indata));
			if( indata != ACK ) push( indata,&input_stack );
		}
		if( !(stat & DRR_MASK) ) { /* MPU is ready to receive data */
			outp(DATAPORT,data);
			if( midi_trace ) printf("put %s\n",midi_ntoa(data));
			result = ACK;
			break;
		}
		if( kbhit() ) {
			result = ERROR;
			break;
		}
	}
	if( debugflag ) printf(", result = %s\n",midi_ntoa(result));
	return result;
}

int send_midi_cmd( int command )
{
	int i,j;
	int stat   = 0;
	int data   = 0;
	int result = ERROR;
	if( debugflag) printf("send_midi_cmd: cmd = %s",midi_ntoa( command ));
	
	/*  first, clear out any data that may be there */
	while( !((inp(STATPORT) & DSR_MASK)) ) {
		data = inp(DATAPORT);
		if( midi_trace ) printf("got %s\n",midi_ntoa(data));
		if( data != ACK ) push(data,&input_stack);
		if( kbhit() ) break;
	}
	
	/* now, output the command */
	outp(COMPORT,command);
	if( midi_trace ) printf("put command %s\n",midi_ntoa(command));
	do_mpu_delay();
	
	/* wait for the ACK to arrive */
	if( command == RST ) result = ACK; /* doesn't, in this case */
	else {
		i = 4000;
		while( i-- ) {
			if( (inp(STATPORT) & DSR_MASK) ) continue; 
			result = inp(DATAPORT);
			if( midi_trace ) printf("got %s\n",midi_ntoa(result));
			if( result == ACK ) break;
			else push(result,&input_stack);
			if( kbhit() ) break;
		}
	}
	do_mpu_delay();
	if( debugflag ) printf(", reply = %s\n",midi_ntoa( result ) );
	return result;
}

int  set_patch( int patch )
{
	if( put_midi_data( PATCH_CH ) == ERROR ) return ERROR;
	return put_midi_data( patch );
}

int  send_key_on( int k, int v, int c )
{
	if( debugflag ) printf("key_on: %4d,%4d,%4d\n",k,v,c);
	if( put_midi_data( KEY_ON | c)  == ERROR ) return ERROR;
	if( put_midi_data( k )          == ERROR ) return ERROR;
	return put_midi_data( v );
}

int  send_key_off( int k, int v, int c )
{
	if( debugflag ) printf("key_off: %4d,%4d,%4d\n",k,v,c);
	if( put_midi_data( KEY_OFF | c )  == ERROR ) return ERROR;
	if( put_midi_data( k )            == ERROR ) return ERROR;
	return put_midi_data( 0 );
}

int  all_notes_off(int ch)
{
	if( debugflag ) printf("all_notes_off\n");
	if( put_midi_data( CTRL_CH | (ch & 0xf) ) == ERROR ) return ERROR;
	if( put_midi_data( ALL_NOTES_OFF )        == ERROR ) return ERROR;
	return put_midi_data( 0 );
}

int  init_midi()
/* puts the MPU into UART mode */
{
	if( send_midi_cmd( RST ) == ERROR ) return ERROR;
	return send_midi_cmd( UART );
}
int  exit_midi()
{
	int channel;
	pause(100);
	for( channel=0;channel<=15;channel++ ) {
		if( channel_used[channel] ) all_notes_off(channel);
	}
	return send_midi_cmd( RST );
}

void push( int b, stack *stk )
{
	if ( stk->top <= 126 ) stk->data[stk->top++] = b;
	/* otherwise ignore the byte */
}

int empty( stack *stk )
{
	return( stk->top == 0 );
}

int pop( stack *stk )
{
	return( stk->top >= 0 ? stk->data[--stk->top] : ERROR );
}

void pause( int n )
/*	Pauses n miliseconds	*/
{
	int i,j;
	for( i = 0; i < n; i++ ) {
		if( tty_abort() ) longjmp(play_done,ERROR);
		for( j = 1; j <= pause_constant; j++ );
	}
}

int adjust_pause(int ms)
{
	float real_ms;
	mark_time();
	pause(ms);
	real_ms = 10.0*check_time();
	pause_constant = (int)(pause_constant*(ms/real_ms));
	return pause_constant;
}

int mark_time()
{
#ifdef MSC 
	_dos_gettime(&time0);
	return 100*time0.second + time0.hsecond;
#endif
#ifdef TURBOC 
	time0 = clock();
#endif
}

int check_time()
/* returns number of deciseconds since mark_time was called.  won't */
/* work for times greater than about 5 minutes (327.67 seconds)     */
{
#ifdef MSC 
	int minute;
	float dtime;
	dtime = 6000.0*time0.minute + 100.0*time0.second + time0.hsecond;
	_dos_gettime(&time1);
	minute = (time0.minute>time1.minute) ? time1.minute + 60 : time1.minute;
	dtime = 6000.0*minute + 100.0*time1.second + 1.0*time1.hsecond - dtime;
	return (int)dtime;
#endif
#ifdef TURBOC 
	float dtime;
	dtime = ((clock() - time0)*1000.0)/CLK_TCK;
	return (int) dtime;
#endif
}

int set_midi_trace(int flag)
{
	midi_trace = flag;
	return midi_trace;
}

#ifdef AMI
int tty_abort()
{
	return( ttypahead() == CTRL_G) );
}
#else
int tty_abort()
{
	int c = ERROR;
	if( kbhit() ) c = bdos(7, 0, 0) & 0xff;
	return( c == CTRL_G );
}
int mlwrite( char *s )
{
	printf("%s\n",s);
}
#endif

