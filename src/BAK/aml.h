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

#define EMPHASIZE (1.25)
#define DEEMPHASIZE (0.75)
#define TUNING_BIAS (-52)		/* bias note numbers for a DX7 */
#define DEFAULT_TEMPO 60.0

/* node types */

#define A_REST		0
#define NOTE_ON		1
#define NOTE_OFF	2
#define A_NOTE		3
#define START_TIE	4
#define END_TIE		5
#define UNDEFINED	15


/* key assignments */
#define OCTAVE_UP	'/'
#define OCTAVE_DOWN	'\\'
#define SHARP		'+'
#define FLAT		'-'
#define NATURAL		'='
#define FORTE		'!'
#define PIANO		'?'
#define TIE		'_'

/* markers for syntactic classes */
#define BEGIN_SEQ	'['
#define END_SEQ		']'
#define BEGIN_SET	'{'
#define END_SET		'}'
#define BEGIN_FUN	'('
#define END_FUN		')'
#define BEGIN_PARAM	'-'

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
char *type_name[] = { "rest","on  ","off ","note","start_tie","end_tie",0,0,0};
char *	program = NULL;	/* name of this program */
int trace	= 0; 
FILE *tfile	= NULL;
FILE *sng_file	= NULL;
FILE *aml_file	= NULL;
FILE *midi_file	= NULL;
int debugflag	= 0;
int level = 0;
int IOflag = 0;
int nnodes	= 0;
/*
 *	Global state determining default duration of a beat, default
 *	volume parameters, and so on
 */
 
float tempo = DEFAULT_TEMPO;	/* tempo in beats per minute.  This */
				/* is the fundamental time keeper. */
				
float msec_per_beat =  (1000.0*60.0)/DEFAULT_TEMPO;
				/* msec_per_beat is milliseconds per */
				/* beat -- a function of the tempo.  */
				
int song_volume	= 64;		/* this is the default value that is  */
				/* converted to a midi velocity figure.*/
				/*  It is multiplied by the entity_volume*/
				/*  and relative_volume to get the final*/
				/*  note velocity. It ranges from 0 to 127*/


/**************************************************************************/
#else
/**************************************************************************/
extern float master_start;
extern char *	program;	/* name of this program */
extern char *type_name[];
extern int trace;
extern FILE *tfile;
extern FILE *sng_file;
extern FILE *aml_file;
extern FILE *midi_file;
extern int debugflag;
extern int level;
int IOflag;
extern int nnodes;
extern float tempo;
extern byte beat_duration_value;
extern float msec_per_beat;
extern int song_volume;

#endif


float	do_accent(char c);
int	song( ENVIRONMENT env);
node *	dur(char c, int *n, ENVIRONMENT *env);
void	param( char c, ENVIRONMENT *env );
node *  basic( char c, ENVIRONMENT *env);
node *	seq( char c, ENVIRONMENT *env );
node *	set( char c, ENVIRONMENT *env );
node *	fun( char c, ENVIRONMENT *env);
node *	dyn( char c, ENVIRONMENT *env);
node *	note( char base_note_name, ENVIRONMENT *env);
node *  element( ENVIRONMENT *env);


void	enter( char *s );
void	error(char *s1, void *s2);
int	get_num();
void	init_io(char *ifname,char *ofname);
void	leavechar( char *s, char c );
void	leavefloat( char *s1, float f);
void	leaveint( char *s, int i );
void	leavestring( char *s1, char *s2 );
int	nextc();
void	output( node *list );
void	parse_error(char *s);
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
void	dmp_node(node *n);
void	dmp_list(node *list);
node *  sort_list(node *list);
node *  car(node **list);

#endif
