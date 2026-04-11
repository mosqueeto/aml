/*
 *	Copyright (c) 1989 by Kent Crispin and Songbird Software.
 *	All rights reserved.
 *
 *	file: io.c
 *	-- contains the i/o interface for the aml compiler
 *
 */

#include "aml.h"
#include "stdlib.h"
#include "ed.h"

static FILE *sng_file,*ml1_file;
static LINE *current_line;
static int *current_ofst;
static char push_stk[16];
static int  push_top = 0;

void init_io()
{
	current_line = curwp->w_dotp;
	current_ofst = curwp->w_doto;
}

int ml1_getc()
{
	int c;
	if( push_top > 16 || current_line == NULL ) return ABORT_COMPILE;
	if( push_top > 0 ) 
		return( push_stk[--push_top] );
	if( current_ofst = llength(current_line) {
		c = '\n';
		current_line = lforw(current_line);
		current_ofst = 0;
	} else {
		c = lgetc(current_line,current_ofst);
		current_ofst++;
	}
	return c;
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
	if( push_top > 16 ) {
		error("Too many characters pushed");
		return COMPILE_ERR;
	}
	push_stk[ push_top++ ] = c;		
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
	/* something on the message line... */
}

#ifdef NEWDS
SongEventSequence *curSequence = 0;	/* Hal 89.02.12 */

void setSongSequence( SongEventSequence * newSequence )
{
	/*
	 *	Check to make sure that only valid transistions are allowed.
	 *	To be valid, exactly one of the new or current pointers must
	 *	be non-zero.
	 */
	if( LXOR(curSequence, newSequence) ) {
		curSequence = newSequence;
		if( trace ) {
			fprintf( stderr, "Current sequence is now %p.\n" );
		}
	} else if( curSequence ) { /* Current Sequence still active! */
		error( "attempt to reset active sequence.\n", NULL );
	} else { /* Current sequence already closed! */
		error( "attempt to close inactive sequence.\n", NULL );
	}
}
#endif

/**output
 *	run down a note list, doing any final calculations, and
 *	writing the results to a file.  Output expects a pointer to
 *	the tail of a circular list.
 */
void output( node *event_list, char *fname )
{
	int d,v,e,nt,c;
	node *n;
	enter("output");
	if( fname != NULL ) {
	}
	n = event_list->next;	
	event_list->next = NULL;/* terminate the list for following scan */
	event_list = n;
	if( trace ) dmp_list(event_list);
	while( event_list != NULL ) {
		n = event_list;
		e = n->event;
		nt = n->note;
		v = (n->volume)*song_volume + 0.5;
		c = n->channel;
		d = (n->delay)*msec_per_beat + 0.5;
		/* print it to the "object file" */
		fprintf(ml1_file,"%4d,%4d,%4d,%4d,%8d\n",e,nt,v,c,d);
		/* and save it to play, if it compiles successfully */
#ifdef NEWDS
		if( !curSequence ) {
			error( "No current sequence!\n", NULL );
		} else if( !(*curSequence->putEvent)(curSequence,e,nt,v,c,d) ) {
			error( "Couldn't insert event.\n", NULL );
		}
#else
		event[output_index] = e;
		note[output_index]  = nt;
		vel[output_index]   = v;
		chan[output_index]  = c;
		dlay[output_index]  = d;
		if( output_index++ >= 5000 ) {
			printf("output list overflow\n");
			output_index--;
		}
#endif
		if( trace ) printf("event: %d, note: %d, vel: %d,dlay: %d\n",
			 		e,nt,v,d);
		event_list = event_list->next;
		free_node(n);
	}
	leave("output");
}	
