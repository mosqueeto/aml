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

static char *input_buffer = NULL;
static char *current_char,*current_line;
static int total_bytes = 0;
static int line_count=1;
//static FILE *sng_file;
//static FILE *aml_file;

static char push_stk[1024];
static int push_top = 0;
static int nchars_parsed = 0;

static float startx = 0.0;
static int duration = 0;

/**init_io
	hide the mechanics of io.
 */
void init_io(char *ifname,char *ofname)
{
	
	/* set up input -- just read the entire file into a buffer */
	
	if( (sng_file = fopen( ifname,"r" )) == NULL ) {
		error("couldn't open %s\n",ifname);
	}
	input_buffer = (char *)malloc(32770);
	if( input_buffer == NULL )
		error("couldn't allocate an input buffer\n",NULL);
	total_bytes = (int)fread(input_buffer,1,32768,sng_file);
	if( !feof(sng_file) )
		error("couldn't read entire file\n",NULL);
	fclose(sng_file);
	*(input_buffer + total_bytes + 0) = '\0';
	*(input_buffer + total_bytes + 1) = '\0';
	*(input_buffer + total_bytes + 2) = '\0';
	current_char = input_buffer;
	current_line = input_buffer;

	/* set up output -- initializing a midifile */
	
	create_midi_file(ofname);

	if( ofname != NULL ) {
		if( (aml_file = fopen(ofname,"w")) == NULL ) {
		error("couldn't create output file %s\n",ofname);
		}
	} else {
		if( (aml_file = fopen("aml.o","w")) == NULL ) {
		error("couldn't create output file aml.o\n",NULL);
		}
	}
 	nchars_parsed = 0;
}

void close_io()
{
	close_midi_file();
}


int aml_getc()
{
	int c;
	if( *current_char == '\0' ) {
		IOflag = EOI;
		return 0;
	}

	if( *current_char == '\n' ) {
		line_count++;
		current_line = current_char+1;
	}
	return *current_char++;
}
int nextc()
{
	int c;
	int in_quote = 0;
another: c = aml_getc();
	if( c == '\"' ) {in_quote = 1 - in_quote; goto another; }
	if( in_quote ) goto another;
	
	if( c == '#' ) while( (c = aml_getc()) != '\n' ) ;
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
	char c;

	cp = current_char - 1;
	printf("%s error in line %d, character is <%c> (decimal %d):\n",
		s,line_count,*cp,*cp);
	cp = current_line;
	while( (*cp != '\n') && (*cp != '\0') ) putchar(*cp++);

	putchar('\n');
	cp = current_char;
	while( cp-- > current_line ) {
		putchar(' ');
	}
	putchar('^');
	putchar('\n');

	c = nextc();
	while( !index("[]{}() ",c) ) c = nextc();
	if( IOflag == EOF ) {
		printf("End of file reached\n");
		exit(1);
	}
}

/**output
 *	run down a node list, doing any final calculations.  Sort
 *	the list, to be sure that the calculations didn't put things
 *	out of order, then write the results to a file. 
 */
void output( node *list )
{
	int d = 0;
	int v = 0;
	int e = 0;
	int nt= 0;
	int c = 0;
	int delay;
	node *np1;
	node *np2;
	node *list1;
	float current_time;
	float end_time;
	enter("output");

	if( list == NULL ) {
		leave("output");
		return;
	}

	if( trace ) dmp_list(list);

	/* first pass -- convert all A_NOTE to NOTE_ON/NOTE_OFF */
	list1 = np1 = car(&list);
	while( np1 != NULL ) {
		switch (np1->type) {
		case A_NOTE:
			np1->type      = NOTE_ON;
			/* insert a NOTE_OFF node... */
			np2 = new_node();
			np2->start    = np1->start +
					 np1->duration * (np1->duty/100.0);
			np2->duration = 0.0;
			np2->volume   = 0.0;
			np2->type     = NOTE_OFF;
			np2->duty     = 100.0;
			np2->channel  = np1->channel;
			np2->note     = np1->note;
			list = insert_node(list,np2 );
			list1 = insert_node(list1,np1);
			break;

		case A_REST:
			np1->type = NOTE_OFF;
			list1 = insert_node(list1,np1);
			break;
		case START_TIE:
			np1->type = NOTE_ON;
			list1 = insert_node(list1,np1);
			break;
		case END_TIE:
			np1->type = NOTE_OFF;
			np1->start    = np1->start +
					 np1->duration * (np1->duty/100.0);
			np1->duration = 0.0;
			np1->volume   = 0.0;
			list = insert_node(list,np1);
			break;
		case NOTE_OFF:
			list1 = insert_node(list1,np1);
			break;
		case NOTE_ON:
			list1 = insert_node(list1,np1);
			break;
		default:
			trace = 1;
			fprintf(stderr,"**node type is %d\n",np1->type);
			fprintf(stderr,"node: ");dmp_node(np1);
			fprintf(stderr,"list: ");dmp_list(list);
			fprintf(stderr,"list1: ");dmp_list(list1);
			fprintf(stderr,"\n\n");
			error("output: unexpected node type\n",NULL);
		}
		np1 = car(&list);
	}

	if( trace ) {
		fprintf(stderr,"final ");
		dmp_list(list1);
	}

	/* now, go through and dump the output */

	current_time = 0.0;
	end_time     = 0.0;
	while( (np1 = car(&list1)) != NULL ) {
		if( end_time < np1->start+np1->duration ) 
			end_time = np1->start+np1->duration;

		e = np1->type;
		nt = np1->note;
		v = MIN(((np1->volume)*song_volume + 0.5),127);
		c = np1->channel;
		if( empty(list1) ) {
			d = (int)((end_time - np1->start) *
				msec_per_beat*(1.0));
		}
		else {
			d = (int)(( (head(list1))->start - np1->start) * 
				msec_per_beat*1.0);
		}

		/* print it to the "object file" */
		if( print_object )
			printf("\n%4d,%4d,%4d,%4d,%8d",e,nt,v,c,d);
		delay = duration;
		duration = d;
		if( e != NOTE_ON && e != NOTE_OFF ) 
			printf("odd event: %d\n",e);
		write_note(delay,e,c,nt,v);
		if( trace ) 
			fprintf(stderr,"%4d,%4d,%4d,%4d,%8d\n",e,nt,v,c,d);

		free_node(np1);
	}

	if( nnodes != 0 ) error("nnodes is not 0\n",NULL);
	leave("output");
}	
