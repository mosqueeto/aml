/*
 *	Copyright (c) 1989 by Kent Crispin and Songbird Software.
 *	All rights reserved.
 *
 *	file: io.c
 *	-- contains the i/o interface for the aml compiler
 *
 */

#include "ml1.h"
#include "stdlib.h"

static char *input_buffer = NULL;
static char *current_char,*current_line;
static int total_bytes = 0;
static int line_count=1;
static FILE *sng_file;
static FILE *ml1_file;

static char push_stk[1024];
static int push_top = 0;
static int nchars_parsed = 0;


/**init_io
 * The input io package hides the mechanics of input.  Mostly
 * important for the stand alone compiler.
 */
void init_io(char *ifname,char *ofname)
{
	/* just read the entire file into a buffer */
	
	if( (sng_file = fopen( ifname,"r" )) == NULL ) {
		error("couldn't open %s\n",ifname);
	}
	input_buffer = (char *)malloc(32768);
	if( input_buffer == NULL )
		error("couldn't allocate an input buffer\n",NULL);
	total_bytes = (int)fread(input_buffer,1,32767,sng_file);
	if( !feof(sng_file) )
		error("couldn't read entire file\n",NULL);
	fclose(sng_file);
	*(input_buffer + total_bytes + 0) = '\n';
	*(input_buffer + total_bytes + 1) = '\0';
	*(input_buffer + total_bytes + 2) = '\0';
	current_char = input_buffer;
	current_line = input_buffer;
	if( ofname != NULL ) {
		if( (ml1_file = fopen(ofname,"w")) == NULL ) {
		error("couldn't create output file %s\n",ofname);
		}
	} else {
		if( (ml1_file = fopen("ml1.o","w")) == NULL ) {
		error("couldn't create output file ml1.o\n",NULL);
		}
	}

	event = (byte*) malloc( 5000*sizeof(byte) );
	note  = (byte*) malloc( 5000*sizeof(byte) );
	vel   = (byte*) malloc( 5000*sizeof(byte) );
	chan  = (byte*) malloc( 5000*sizeof(byte) );
	dlay  = (int *) malloc( 5000*sizeof( int) );
 	output_index = 0;
 	nchars_parsed = 0;
}


int ml1_getc()
{
	int c;
	if( push_top > 1024 )
		return COMPILE_ERR;
	else if( push_top > 0 ) 
		c = push_stk[--push_top];
	else
		return *current_char++;
}
int nextc()
{
	int c;
	int in_quote = 0;
another: c = ml1_getc();
	if( c == '\"' ) {in_quote = 1 - in_quote; goto another; }
	if( in_quote ) goto another;
	
	if( c == '#' ) while( (c = ml1_getc()) != '\n' ) ;
	if( c == '\n')	c = ' ';
	if( c == '|') c = ' ';
	return( c );
}

int pushc(char c)
{
	current_char--;
}

/**get_num
 *	parses a number from the input stream and leaves the current
 *	character pointer at the next character.  If no digits are
 *	found at all, it returns 0.  In most cases this is an error...
 *	A single "-" or "+" is not a legal number, so in this case
 *	get_num backs up the character pointer to before the offending
 *	character, and returns 0.
 */
int get_num()
{
	char c;
	int t = 0;
	int sign = 1;
	enter("get_num");
	while( isspace( c = nextc() ) );
	if( c == '-' ) {
		sign = -1;
		c = nextc();
		if( !isdigit(c) ) {
			pushc(c); pushc('-');
			return( 0 );
		}
	}
	else if( c == '+' ) {
		c = nextc();
		if( !isdigit(c) ) {
			pushc(c); pushc('+');
			return( 0 );
		}
	}
	while( isdigit( c ) ) {
		t = t*10 + c - '0';
		c = nextc();
	}
	pushc(c);
	t *= sign;
	leaveint("get_num",t);
	return( t );
}

/**parse_error
 *	report a parsing error
 */
void parse_error(char *s)
{
	char *cp;
	printf("parsing error in line %d:\n",line_count);
	cp = current_line;
	while( (*cp != '\n') && (*cp != '\0') ) putchar(*cp++);
		cp = current_char;
	putchar('\n');
	while( cp-- > current_line ) {
		putchar(' ');
	}
	putchar('^');
	putchar('\n');
	printf("%s\n",s);
}

/**output
 *	run down a node list, doing any final calculations, and
 *	writing the results to a file. 
 */
void output( node *n )
{
	int d,v,e,nt,c;
	node *np;
	float current_time = 0.0;
	enter("output");
	if( n == NULL ) return;
	if( trace ) dmp_list(n);
	while( n != NULL ) {
		if( trace ) dmp_list(n);
		if( n->type == A_NOTE ) {
			/* must insert a NOTE_OFF node, and change this to
			 * a NOTE_ON 
			 */
			n->type      = NOTE_ON;
			np = new_node();
			np->start    = n->start + n->delay * (n->duty/100.0);
			np->delay    = 0.0;
			np->volume   = 0.0;
			np->type     = NOTE_OFF;
			np->duty     = n->duty;
			np->channel  = n->channel;
			np->note     = n->note;
			(void)insert_node( n,np );
		}
		e = n->type;
		nt = n->note;
		v = (n->volume)*song_volume + 0.5;
		c = n->channel;
		d = (n->start - current_time)*msec_per_beat + 0.5;
		current_time = n->start;

		/* print it to the "object file" */
		fprintf(ml1_file,"%4d,%4d,%4d,%4d,%8d\n",e,nt,v,c,d);

		/* save it to play, if it compiles successfully */
		event[output_index] = e;
		note[output_index]  = nt;
		vel[output_index]   = v;
		chan[output_index]  = c;
		dlay[output_index]  = d;
		if( output_index++ >= 5000 ) {
			printf("output list overflow\n");
			output_index--;
		}
		if( trace ) fprintf(stderr,
			"event: %d, note: %d, vel: %d,dlay: %d\n",e,nt,v,d);
		np = n;
		n = n->next;
		free_node(np);
	}
	if( nnodes != 0 ) error("nnodes is not 0",NULL);
	leave("output");
}	
