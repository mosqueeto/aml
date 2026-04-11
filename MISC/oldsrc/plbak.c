#include "ctype.h"
#include "stdio.h"
#include "conio.h"
#include "dos.h"
#include "process.h"

/*	MPU-401 codes	*/
	  
#define	  DATAPORT  0x330
#define	  STATPORT  0x331
#define	  COMPORT   0x331
#define	  DSR_MASK  0x080
#define	  DRR_MASK  0x040
#define	  STAT_MSK  0x0C0
#define	  ACK       0x0FE
#define	  RST       0x0FF
#define	  DISM      0x08B
#define	  WTSD      0x0D0
#define	  UART      0x03F

/*	MIDI codes	*/
	  
#define	NOTE_OFF	0x080
#define	NOTE_ON		0x090
#define	POLY_KP_AT	0x0A0
#define	CTRL_CH		0x0B0
#define	PATCH_CH	0x0C0
#define	CHNL_KP_AT	0x0D0
#define WHEEL_CH	0x0E0
#define SYSTEM		0x0F0

#define ALL_NOTES_OFF	0x07B

typedef unsigned char byte;

/*	Stack Definitions	*/

#define  MAXSTACK  126
typedef struct {
	byte top;
	byte data[127];
} stack;

#define DEBUG 0

#define TRUE 1
#define FALSE 0

stack *input_stack;

int debug = 1; 

FILE *sng_file;

void pause( int n );
void send_mpu_cmd( byte command );
void set_patch( byte patch );
void send_key_on( int k, int v, int c );
void send_key_off( int k, int v, int c );
void init_midi();
void error(char *s1, char *s2);

int dlay[5000],event[5000];
byte note[5000],vel[5000],chan[5000];
main(argc,argv)
int argc;
char *argv[];
{
	int i=0;
	int j;
	char fname[20];
	init_midi();
	if( argc <= 1 ) {
		if( (sng_file = fopen( "ml1.o","r" )) == NULL ) 
			error("PL: couldn't open ml1.int\n",NULL);
	} else {
		if( (sng_file = fopen( argv[1],"r" )) == NULL )
			error("PL: couldn't open input file %s\n",argv[1]);
	}
	while( fscanf(sng_file,"%d,%d,%d,%d,%d",
		&event[i],&note[i],&vel[i],&chan[i],&dlay[i])==5 ) {
		i++;
	}
	for(j=0;j<=i;j++) {
#if DEBUG
		printf("e=%2d,n=%4d(%3x),v=%4d(%3x),c=%2d,d=%6d\n",
		event[j],note[j],note[j],vel[j],vel[j],chan[j],dlay[j]);
#endif
		pause(dlay[j]);
		if( event[j] == 1 ) send_key_on(note[j],vel[j],chan[j]);
		if( event[j] == 2 ) send_key_off(note[j],vel[j],chan[j]);
		if( event[j] == 3 ) ;
		if( event[j] == 4 ) ;
	}
	fclose(sng_file);

}
void push(byte b, stack *stk )
{
	if ( stk->top <= MAXSTACK ) 
	{
		stk->data[stk->top++] = b;
	}
	else 
	{
	/* ignore the byte */
	}
}


stack *newstack()
{
	stack *stk;
	stk = (stack *)malloc(sizeof(stack));
	stk->top = 0;
	return( stk );
}


int empty( stack *stk )
{
	return( stk->top == 0 );
}


byte pop( stack *stk )
{

	if( stk->top >= 0 ) {
		return( stk->data[--stk->top]);
	}
	else {
		printf("Stack underflow\n");
		exit(0);
	}
	return( '\000' );
}

void pause( int n )
/*	Pauses n miliseconds	*/
/*	The clock constant is 	*/
/*	505 at 8 mhz 		*/
{
	int i,j;
	for( i = 1; i <= n; i++ )
		for( j = 1; j <= 245; j++ );
}

char *mpu_ntoa( byte b )
{
static char temp[10];
char *tp;
	if( b== ACK ) 			tp = "ACK     ";
	else if( b==RST	) 		tp = "RST     ";
	else if( b==DISM ) 		tp = "DISM    ";
	else if( b==WTSD ) 		tp = "WTSD    ";
	else if( b==UART ) 		tp = "UART    ";
	else if( (b & 0xf0)==NOTE_ON )	tp = "NOTE_ON ";
	else if( (b & 0xf0)==NOTE_OFF )	tp = "NOTE_OFF";
	else if( b==PATCH_CH ) 		tp = "PATCH_CH";
	else {
		sprintf( temp,"%8x",b );  
		tp = temp;
	}
	return( tp );
}

byte get_mpu_data( )
{
	byte data;
	int i;
#if DEBUG
printf("get_mpu_data");
#endif
	if( ! empty(input_stack) )
	{
  		data = pop(input_stack);
  	}
	else
	{
		while( inp(STATPORT) & DSR_MASK ) ;
		data = inp(DATAPORT);
	}
#if DEBUG
printf(": data = %s\n",mpu_ntoa( data) );
#endif
	return( data );
}

void put_mpu_data( byte data )
{
	byte s,t;
	int waiting,i;

#if DEBUG	
printf("put_mpu_data: data = %s\n",mpu_ntoa(data));
#endif
	waiting = TRUE;
	while( waiting ) 
	{
		s = inp(STATPORT);
		if( (s & STAT_MSK) != STAT_MSK )
		{
			if( (s & DRR_MASK) != DRR_MASK )
			{
				outp(DATAPORT,data);
				waiting = FALSE;
			}
			if( (s & DSR_MASK) != DSR_MASK )
			{
				t = inp(DATAPORT);
				if( t != ACK )
				{
					push( t,input_stack );
				}
			}
		}
	}
}

void send_mpu_cmd( byte command )
{
	byte reply;
	int i;

#if DEBUG	
printf("send_mpu_cmd: cmd = %s\n",mpu_ntoa( command ));
#endif
	while( inp(STATPORT) & DRR_MASK ) ;
	outp(COMPORT,command);
	reply = 0xFF;
	while( reply != ACK )
	{
		while( inp(STATPORT) & DSR_MASK ) ;
		reply = inp(DATAPORT);  /*get ACK*/
	}
}


void set_patch( byte patch )
{
	send_mpu_cmd( WTSD );
	put_mpu_data( PATCH_CH );
	put_mpu_data( patch );
}

void send_key_on( int k, int v, int c )
{
#if DEBUG
	printf("send_key_on:  %4x, %4x, %4x\n",k,v,c);
#endif
	put_mpu_data( NOTE_ON | c );
	put_mpu_data( k );
	put_mpu_data( v );
}

void send_key_off( int k, int v, int c )
{
#if DEBUG
	printf("send_key_off: %4x, %4x, %4x\n",k,v,c);
#endif
	put_mpu_data( NOTE_OFF | c );
	put_mpu_data( k );
	put_mpu_data( 0 );
}

void all_notes_off()
{
	put_mpu_data( CTRL_CH );
	put_mpu_data( ALL_NOTES_OFF );
}

void init_midi()
/*
 *	puts the MPU into UART mode
 */
{
	input_stack = newstack();
	send_mpu_cmd( RST );
	send_mpu_cmd( UART );
}


void error(char *s1,char *s2 )
{
	printf(s1,s2);
	exit(1);
}
