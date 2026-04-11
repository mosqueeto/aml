/*
 *	file: aml.h
 *		-- header file for the aml compiler
 *
 *	Copyright (c) 1989 by Kent Crispin and Songbird Software.
 *	All rights reserved.
 *
 */

#define AML

#ifdef AMLH_DEFD
#else
#define AMLH_DEFD 1

#include <ctype.h>
#include <stdio.h>
#include "midifile.h"

#define TRUE	1
#define FALSE	0

#define ERROR	-1
#define OK	1

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#define EMPHASIZE (1.15)
#define DEEMPHASIZE (0.87)
#define TUNING_BIAS (-52)		/* bias note numbers for a DX7 */
#define DEFAULT_TEMPO 60

/* node types */

#define A_REST		0
#define NOTE_ON		1
#define NOTE_OFF	2
#define A_NOTE		3
#define UNDEFINED	15


/* key assignments */
#define OCTAVE_UP	'/'
#define OCTAVE_DOWN	'\\'
#define SHARP		'+'
#define FLAT		'-'
#define NATURAL		'='
#define BEGIN_SEQ	'['
#define END_SEQ		']'
#define BEGIN_SET	'{'
#define END_SET		'}'
#define FORTE		'!'
#define PIANO		'?'
#define TIE		'_'
#define BEGIN_PARAM	'-'
#define BEGIN_FUNC	'('
#define END_FUNC	')'

/*
 * 	misc constants
 */
#define EOI 		-1
#define DID_NOTES	1
#define NO_NOTES	2

/*	A few macros
 */
#define round(f) ( (int) (f+0.5) )
#define is_real_note(c) ( ((tolower(c) >= 'a') && (tolower(c) <= 'g')) )
#define is_note_name(c) ( is_real_note(c) || (tolower(c)=='r') || (c=='_') )

typedef unsigned char byte;	/* basically a small unsigned integer */


/*	"nodes" are the compiled representation of musical events, primarily
 *	notes, though some of the fields may be overlaid with other semantics
 */
struct node_struct {
	int	n;
	int	type;
	struct node_struct *next;
	float	start;
	float	duration;
	float	delay;
	float	volume;
	byte	duty;
	byte	channel;
	byte	note;
};
typedef struct node_struct node;

struct scope_env {
	float	start;
	float	duration;
	float	volume;
	byte	tempo;
	byte	velocity;
	byte	channel;
	char	key[7];	/* really signed */
	byte	octave;
	byte	duty;
	char	transpose;
};
typedef struct scope_env ENVIRONMENT;

/***********************************************************************/
#ifdef MAIN
/***********************************************************************/
/*
 *	Set up the default environment
 */
ENVIRONMENT song_env =
	{ 
	  0.0,			/* start */
	  1.0,			/* duration */
	  1.0, 			/* volume */
	  0,			/* tempo */
	  0,			/* velocity */
	  0,			/* channel */
	  {0,0,0,0,0,0,0}, 	/* key signature */
	  4,			/* octave */
	  90,			/* duty */
	  64,			/* transpose */
	};

float master_start = 0.0;
char *type_name[] = { "rest","on  ","off ","note"};
char *	program = NULL;	/* name of this program */
int trace	= 0; 
FILE *tfile	= NULL;
FILE *sng_file	= NULL;
FILE *aml_file	= NULL;
FILE *midi_file	= NULL;
int debugflag	= 0;
unsigned int *nullp = NULL;	/* for debugging purposes */
int nnodes	= 0;
/*
 *	Global state determining default duration of a beat, default
 *	volume parameters, and so on
 */
 
int tempo = DEFAULT_TEMPO;	/* tempo in beats per minute.  This */
				/* is the fundamental time keeper. */
				
int msec_per_beat =  (1000*60)/DEFAULT_TEMPO;	/* Hal 89.01.13 */
				/* msec_per_beat is milliseconds per */
				/* beat -- a function of the tempo.  */
				
int song_volume	= 64;		/* this is the default value that is  */
				/* converted to a midi velocity figure.*/
				/*  It is multiplied by the entity_volume*/
				/*  and relative_volume to get the final*/
				/*  note velocity. It ranges from 0 to 127*/

byte 	*event	= NULL;		/* parallel arrays that store a compiled*/
byte	*note	= NULL;		/* song before it is played */
byte	*vel	= NULL;
byte	*chan	= NULL;
int 	*dlay	= NULL;
int 	output_index = 0;

/**************************************************************************/
#else
/**************************************************************************/
extern float master_start;
extern	char *	program;	/* name of this program */
extern char *type_name[];
extern int trace;
extern FILE *tfile;
extern FILE *sng_file;
extern FILE *aml_file;
extern FILE *midi_file;
extern int debugflag;
extern unsigned int *nullp;
extern int nnodes;
extern int tempo;
extern byte beat_duration_value;
extern int msec_per_beat;
extern int song_volume;

extern byte *event,*note,*vel,*chan;
extern int *dlay;
extern int output_index;
#endif


float	do_accent(char c);
node *	do_set( ENVIRONMENT env );
int	do_entity( ENVIRONMENT *env);
node *	do_fun( ENVIRONMENT *env);
float	do_named_duration(char named_duration);
node *	do_note( char base_note_name, ENVIRONMENT *env);
void	do_parameter( ENVIRONMENT *env );
int	do_relative_duration(char c);
node *	do_seq( ENVIRONMENT env );
void	do_song( ENVIRONMENT env );

void	enter( char *s );
void	error(char *s1, void *s2);
int	get_num();
void	init_io(char *ifname,char *ofname);
int	is_named_duration( char c );
void	leavechar( char *s, char c );
void	leavefloat( char *s1, float f);
void	leaveint( char *s, int i );
void	leavestring( char *s1, char *s2 );
int	nextc();
void	output( node *list );
void	parse_error(char *s);
int	play(int repeats);
int	pushc(char c);
node *	first_node( node * List );
node *	last_node( node * List );
node *	append_node(node *List, node *Node);
node *	append_list( node* List, node *appendee);
node *	merge_lists( node * list1, node * list2);
node *	new_node();
void	free_node(node *n);
void	free_list(node *l);
node *	insert_after_node( node *List, node *Node, node *insertee );
node *	insert_node( node *List, node *Node );
node *	head( node *List );
node *	tail( node *List );

#endif
