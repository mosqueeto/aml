/*	note.c -- compile a note
 *	
 *	copyright (c) 1989 by Songbird Software and Kent Crispin
 *			All rights reserved.
 *
 */

#include "aml.h"

static int ties = 0;
int get_note_number(char base_note_name, ENVIRONMENT * env);

/*
 * The note_table is used to convert characters in the range of 'a' to 
 * 'g' to an integer.  In standard music notation, octaves begin with a
 * 'c', and not with an 'a', so 'abcdefg' must be shifted to 'cdefgab'.
 */
int note_table[7] = { 9,11,0,2,4,5,7 };
 
/**do_note
 *	Generate a node for a note.
 */
node *note( char base_note_name, ENVIRONMENT *env )
{
	char c;
	int note;
	int start_tie = FALSE;
	int end_tie = FALSE;
	node *note_node;
	
	enter("note");

	if( base_note_name == TIE ) {	/* a suspension across seq bounds */
		end_tie = TRUE;
		ties--;
		if( ties < 0 ) {
			parse_error("dangling end of tie");
			ties++;
			return NULL;
		}
		base_note_name = nextc();
		if( !is_real_note(base_note_name) ) {
			parse_error("Incorrect note name");
			return NULL;
		}
	}
	
	note = get_note_number(base_note_name, env);

	/* are we starting a tie? */
	if( (c = nextc()) == TIE ) {
		start_tie = TRUE;
		ties++;
	}
	else pushc(c);

	if( start_tie && end_tie ) return NULL;	/* nothing to do */
	
	/* allocate a new node and initialize it */
	note_node = new_node();
	note_node->next = note_node;
	note_node->start    = env->start;
	note_node->duration = env->duration;
	note_node->volume   = env->volume;
	note_node->duty     = env->duty;
	note_node->channel  = env->channel;
	note_node->note     = note;
	if( start_tie )     note_node->type = START_TIE;
	else if( end_tie )  note_node->type = END_TIE;
	else 		    note_node->type = A_NOTE;
	if( note == 0 ) note_node->type = A_REST;
	else current_note = note;
	leaveint("note",note_node->note);
	return note_node;
}


/**get_note_number
 *	Convert a note name to a note number, accounting for the 
 *	current default values for accidental and octave.
 */
int get_note_number(char base_note_name, ENVIRONMENT *env)
{
	int base_note,note;
	int rel_oct = 0;
	byte acc_flag = FALSE;
	char note_acc = 0;
	char c;

	enter("get_note_number");
	if( base_note_name == 'r' ) {
		note = 0;
		goto rest;
	}
	base_note = base_note_name - 'a';
	
	/* Check for an accidental on the note
	 */
	c = nextc();
	if( c == SHARP ) {
		note_acc = +1; 
		acc_flag = TRUE;
		c = nextc();
		if( c == SHARP ){	/* must check for double sharp */
			note_acc = +2;
			c = nextc();
		}
	}
	else if( c == FLAT ) { 
		note_acc = -1;
		acc_flag = TRUE;
		c = nextc();
		if( c == FLAT ){		/* and check for double flat */
			note_acc = -2;
			c = nextc();
		}
	}
	else if( c == NATURAL ) { 	/* fortunately, no double natural */
		note_acc = 0;
		acc_flag = TRUE;
		c = nextc(); 
	}
	
	/* Convert to a relative pitch, in the range from c-- to b++.
	 * Add 2 so we stay in array bounds...
	 */
	if( acc_flag ) {	/* the note was modified */
		note = note_table[base_note] + note_acc + 2;
	} else {
		note = note_table[base_note] + env->key[base_note] + 2;
	}
	/* get octave designator, if present 
	 */
	if( isdigit( c ) ) {		/* absolute octave present    */
		env->octave = c - '0'; /* convert from char to number  */
		c = nextc();		/* get another char             */
	}
	/*
	 * Account for any relative octave designators
	 */
	while( (c == OCTAVE_UP) || (c==OCTAVE_DOWN) ) {
		if( c==OCTAVE_UP )   rel_oct++;
		if( c==OCTAVE_DOWN ) rel_oct--;	/* Hal 89.01.13 */
		c = nextc();
	}
	pushc(c);	/* return the character for next guy */		
	

	/* Now we have all the information to calculate the final note number,
	 * which should be the actual midi code for the note we want.  Thus,
	 * have to compensate for the actual key assignment of the synth, and
	 * of course adjust for the array bounds conversion.
	 */
	note = (env->octave+rel_oct)*12 + 
			note + TUNING_BIAS + env->transpose - 2;
rest:	
	leaveint("get_note_number",note);
	return note;
}

/**diatonic_step
 *  Move a MIDI note by 'steps' diatonic scale degrees (positive=up,
 *  negative=down) according to the current key signature.
 *  Returns the new MIDI note number, or midi_note unchanged if the
 *  pitch class is not on a scale degree.
 */
int diatonic_step(int midi_note, int steps, ENVIRONMENT *env)
{
    int i, degree, oct, pc, raw;
    /* Build scale: 7 semitone values sorted by pitch (c d e f g a b).
     * note_table indices: a=0 b=1 c=2 d=3 e=4 f=5 g=6             */
    static const int degree_order[7] = {2,3,4,5,6,0,1};
    int scale[7];
    for( i = 0; i < 7; i++ ) {
        int idx = degree_order[i];
        scale[i] = ((note_table[idx] + env->key[idx]) % 12 + 12) % 12;
    }

    /* Decompose midi_note into octave and pitch class */
    raw = midi_note - TUNING_BIAS - env->transpose;
    oct = raw / 12;
    pc  = raw % 12;
    if( pc < 0 ) { pc += 12; oct--; }

    /* Find the scale degree */
    degree = -1;
    for( i = 0; i < 7; i++ ) {
        if( scale[i] == pc ) { degree = i; break; }
    }
    if( degree < 0 ) return midi_note;  /* not on scale -- return unchanged */

    /* Apply steps with octave wrap */
    degree += steps;
    while( degree >= 7 ) { degree -= 7; oct++; }
    while( degree <  0 ) { degree += 7; oct--; }

    return oct * 12 + scale[degree] + TUNING_BIAS + env->transpose;
}

/**dot_note
 *  Parse a '.' (current note) with optional suffix '/' or '\' modifiers
 *  for diatonic stepping.  Each '/' steps up one scale degree; each '\'
 *  steps down one.  Updates current_note and returns a note node.
 */
node *dot_note(ENVIRONMENT *env)
{
    int c, steps = 0;
    node *np;

    while( (c = nextc()) == OCTAVE_UP || c == OCTAVE_DOWN ) {
        if( c == OCTAVE_UP ) steps++;
        else                 steps--;
    }
    pushc(c);

    current_note = diatonic_step(current_note, steps, env);

    np           = new_node();
    np->next     = np;
    np->start    = env->start;
    np->duration = env->duration;
    np->volume   = env->volume;
    np->duty     = env->duty;
    np->channel  = env->channel;
    np->note     = current_note;
    np->type     = A_NOTE;
    return np;
}

void set_key(ENVIRONMENT *env)
{
	char c;
	int base_note;
	c = ' ';
	while( isspace(c = nextc()) );
	if( is_note_name(c) ) {
		base_note = c - 'a';
		c = nextc();
		if( c == '=' ) 
			env->key[base_note] = 0;
		else if( c == SHARP ) 
			while( c == SHARP ) {
				env->key[base_note]++;
				c = nextc();
			}
		else if( c == FLAT ) 
			while( c == FLAT ) {
				env->key[base_note]--;
				c = nextc();
			}
		else if( c == NATURAL ) {
			env->key[base_note] = 0;
			c = nextc();
		}
		else {
			parse_error("improper accidental setting");
		}
	}
	
	else {
		parse_error("not a note name");
	}
}

