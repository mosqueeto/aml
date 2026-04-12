#include "aml.h"

extern int note_table[7];
extern int get_note_number(char base_note_name, ENVIRONMENT *env);

node *fn_add(void);
node *fn_sub(void);
node *fn_mul(void);
node *fn_div(void);
node *fn_def(void);
node *fn_tempo(void);
node *fn_volume(void);
node *fn_turn(ENVIRONMENT *env);
node *fn_cresc(ENVIRONMENT *env);
node *fn_decresc(ENVIRONMENT *env);
node *fn_rpt(ENVIRONMENT *env);

/* fn_element_count: set by multi-element functions so seq() can count
 * each consumed note/event separately rather than the whole call as 1.
 */
int fn_element_count = 1;

/*	types of the return values of functions */

#define NUM_TYPE	0
#define NODE_TYPE	1
#define NULL_TYPE	3

struct fn_descriptor {
	char *	name;
	int	type;
	void	*(*ex)(void);
} fn_table[128];
#if 0
 = {
	{"+",		NUM_TYPE,	fn_add},
	{"-",		NUM_TYPE,	fn_sub},
	{"*",		NUM_TYPE,	fn_mul},
	{"/",		NUM_TYPE,	fn_div},
	{"def",		NUM_TYPE,	fn_def},
	{"tempo",	NUM_TYPE,	fn_tempo},
	{"volume",	NUM_TYPE,	fn_volume},
	{(char *)NULL,	NULL_TYPE,	(int (*)())NULL}
};
#endif

char current_function[32]; 
	
/**get_function_name
 *	Get the function name, terminated by white space or an
 *	"end function" character.
 */
void get_function_name()
{
	int i = 0;
	int c;
	while( !isspace( c = nextc() ) && i < 32 && (c != END_FUN ) ) {
		current_function[i++] = c;
	}
	current_function[i] = '\0';
}

/**fun
 *	Parse and execute any functions.  
 *	It is not strictly accurate to call these things "functions" -- they
 *	are sometimes functions and sometimes macros.  
 *	It is assumed that the "begin function" character has been parsed, 
 *	and the function invocation will be completed when we see the closing 
 *	"end function" character.  The next token (any string of up to 31 
 *	non-whitespace characters) is the function name.  Certain names are 
 *	built in -- basic arithmetic, environment modification, environment 
 *	inquiry, random selection, and so on.  Functions (really macros) can 
 *	be defined.  If the function name isn't a built-in, the list of 
 *	defined functions is scanned.  If the name isn't found, a compile 
 *	error occurs.  If the name is found,  the arguments are evaluated, 
 *	and the strings that result are stuffed into the string that defines 
 *	the function.  This total string is pushed into the input stream 
 *	(except in special cases) and evaluated again...
 *
 */
node *fun( char c, ENVIRONMENT *env )
{
	int i = 0;
	node * np = NULL;
	enter("fun");

	if( c != BEGIN_FUN ) {
		error("fun: bad beginning\n",NULL);
	}
	get_function_name();
	if( strcmp(current_function, "turn") == 0 ) {
		np = fn_turn(env);
	} else if( strcmp(current_function, "rpt") == 0 ) {
		np = fn_rpt(env);
	} else if( strcmp(current_function, "cresc") == 0 ) {
		np = fn_cresc(env);
	} else if( strcmp(current_function, "decresc") == 0 ) {
		np = fn_decresc(env);
	} else {
		while( fn_table[i].name != NULL ) {
			if( strcmp( current_function,fn_table[i].name ) == 0 ) {
				np = (void *)fn_table[i].ex();
				break;
			}
			i++;
		}
	}
	leave("fun");
	return np;
}

node *fn_add(void)
{
	printf("fn_add called\n");
	return NULL;
}
node *fn_sub(void)
{
	printf("fn_sub called\n");
	return NULL;
}
node *fn_mul(void)
{
	printf("fn_mul called\n");
	return NULL;
}
node *fn_div(void)
{
	printf("fn_div called\n");
	return NULL;
}
node *fn_def(void)
{
	printf("fn_def called\n");
	return NULL;
}
node *fn_tempo(void)
{
	printf("fn_tempo called\n");
	return NULL;
}
node *fn_volume(void)
{
	printf("fn_volume called\n");
	return NULL;
}

/**get_int_arg
 *	Very much like get_num, except that ...
 */
int get_int_arg(void)
{
    return 0;
}

#define MAX_FN_ELEMS 256

/**copy_list_with_offset
 *	Duplicate a circular node list, shifting every node's start time
 *	by start_offset.  new_node() is used so nnodes stays consistent.
 */
static node *copy_list_with_offset(node *list, double start_offset)
{
    node *result = NULL;
    node *np = list;
    do {
        np = np->next;
        node *cp = new_node();
        int saved_n = cp->n;
        *cp = *np;          /* copy all fields */
        cp->n    = saved_n; /* restore node number assigned by new_node */
        cp->next = cp;      /* make singleton before inserting */
        cp->start += start_offset;
        result = append_list(result, cp);
    } while( np != list );
    return result;
}

/**fn_rpt
 *	Repeat a group of events n times.  Syntax:
 *	    (rpt n events...)
 *
 *	Flat events: (rpt 3 a b c) -- each note gets one seq slot per occurrence;
 *	             fn_element_count = n*k, so timing is fully preserved.
 *	Wrapped seq: (rpt 3 [a b c]) -- each repetition gets one seq slot;
 *	             fn_element_count = n*1.
 */
static node *build_rpt(ENVIRONMENT *env, int n_reps)
{
    node *sub = NULL;
    int k = 0;
    char c;
    ENVIRONMENT inner_env;

    if( n_reps > MAX_FN_ELEMS ) {
        parse_error("rpt: repeat count exceeds maximum");
        n_reps = MAX_FN_ELEMS;
    }

    inner_env = *env;
    inner_env.duration = 1.0;

    /* Parse one repetition into sub, counting k non-null elements */
    while(1) {
        while( isspace(c = nextc()) );
        if( c == END_FUN ) break;
        inner_env.start = env->start + (double)k;
        node *np = basic(c, &inner_env);
        if( np != NULL ) {
            sub = append_list(sub, np);
            k++;
        }
    }

    if( k == 0 || sub == NULL ) { fn_element_count = n_reps; return NULL; }

    fn_element_count = n_reps * k;

    /* append_list modifies the tail node's ->next pointer, which would corrupt
     * sub as a copy source.  So build ALL copies first while sub is intact,
     * then link them together.
     */
    node *segs[MAX_FN_ELEMS];
    segs[0] = sub;
    for( int i = 1; i < n_reps; i++ ) {
        segs[i] = copy_list_with_offset(sub, (double)(i * k));
    }
    node *list = segs[0];
    for( int i = 1; i < n_reps; i++ ) {
        list = append_list(list, segs[i]);
    }

    return list;
}

node *fn_rpt(ENVIRONMENT *env)
{
    char c;
    int n_reps;
    while( isspace(c = nextc()) );
    if( isdigit(c) ) {
        pushc(c);
        n_reps = get_num();
    } else {
        pushc(c);
        n_reps = 2;
    }
    if( n_reps <= 0 ) {
        parse_error("rpt: repeat count must be > 0");
        while( nextc() != END_FUN );
        return NULL;
    }
    return build_rpt(env, n_reps);
}

/**parse_vol_spec
 *	Read an optional run of '!' or '?' characters from the stream and
 *	return the resulting volume multiplier applied to env->volume.
 *	If neither character is seen, pushes the character back and returns
 *	default_vol unchanged.
 */
static double parse_vol_spec(ENVIRONMENT *env, double default_vol)
{
    char c;
    double vol;
    while( isspace(c = nextc()) );
    if( c == FORTE ) {
        vol = env->volume;
        do { vol *= EMPHASIZE; } while( (c = nextc()) == FORTE );
        pushc(c);
    } else if( c == PIANO ) {
        vol = env->volume;
        do { vol *= DEEMPHASIZE; } while( (c = nextc()) == PIANO );
        pushc(c);
    } else {
        pushc(c);
        vol = default_vol;
    }
    return vol;
}

/**build_cresc
 *	Core of cresc/decresc: read events until ')' and apply a linear
 *	volume ramp from vol_start to vol_end across them.
 *
 *	- Multiple flat events (cresc a b c): each gets one ramp step;
 *	  fn_element_count is set to N so seq() preserves their timing.
 *	- Single wrapped sequence (cresc [a b c]): ramp is applied across
 *	  the individual note nodes; fn_element_count stays 1.
 */
static node *build_cresc(ENVIRONMENT *env, double vol_start, double vol_end)
{
    node *elems[MAX_FN_ELEMS];
    int n = 0;
    char c;
    ENVIRONMENT inner_env;

    inner_env = *env;
    inner_env.duration = 1.0;

    while(1) {
        while( isspace(c = nextc()) );
        if( c == END_FUN ) break;
        if( n >= MAX_FN_ELEMS ) {
            parse_error("cresc: too many elements");
            while( (c = nextc()) != END_FUN );
            break;
        }
        inner_env.start = env->start + (double)n;
        elems[n++] = basic(c, &inner_env);
    }

    fn_element_count = (n > 0) ? n : 1;

    if( n == 0 ) return NULL;

    if( n == 1 ) {
        /* Single element: ramp volume across the individual nodes */
        node *list = elems[0];
        if( list == NULL ) return NULL;
        int nc = 0;
        node *np = list;
        do { np = np->next; nc++; } while( np != list );
        np = list;
        for( int i = 0; i < nc; i++ ) {
            np = np->next;
            np->volume = (nc <= 1) ? (vol_start + vol_end) * 0.5 :
                         vol_start + (vol_end - vol_start) * i / (nc - 1);
        }
        return list;
    }

    /* Multiple elements: one volume level per element */
    node *list = NULL;
    for( int i = 0; i < n; i++ ) {
        if( elems[i] == NULL ) continue;
        double vol = vol_start + (vol_end - vol_start) * i / (n - 1);
        node *np = elems[i];
        do {
            np = np->next;
            np->volume = vol;
        } while( np != elems[i] );
        list = append_list(list, elems[i]);
    }
    return list;
}

/**fn_cresc / fn_decresc
 *	Crescendo and decrescendo.  Syntax:
 *	    (cresc events...)           -- ramp DEEMPHASIZE -> EMPHASIZE
 *	    (cresc ?? !!! events...)    -- explicit start/end dynamic levels
 *	    (decresc events...)         -- ramp EMPHASIZE -> DEEMPHASIZE
 *
 *	Volume specs use '!' and '?' exactly as the dynamic modifier does:
 *	each '!' multiplies by EMPHASIZE, each '?' by DEEMPHASIZE.
 *	Two specs are read; if a spec is absent its default is used.
 */
node *fn_cresc(ENVIRONMENT *env)
{
    double vol_start = parse_vol_spec(env, env->volume * DEEMPHASIZE);
    double vol_end   = parse_vol_spec(env, env->volume * EMPHASIZE);
    return build_cresc(env, vol_start, vol_end);
}

node *fn_decresc(ENVIRONMENT *env)
{
    double vol_start = parse_vol_spec(env, env->volume * EMPHASIZE);
    double vol_end   = parse_vol_spec(env, env->volume * DEEMPHASIZE);
    return build_cresc(env, vol_start, vol_end);
}

/**fn_turn
 *	Generate a musical turn ornament on a note.
 *	Syntax: (turn <note>)  e.g. (turn c), (turn b-)
 *
 *	Produces 4 notes, each 1/4 of the current duration:
 *	    upper diatonic neighbor, main note, lower diatonic neighbor, main note
 *
 *	Neighbors are diatonic -- they use note_table[] + env->key[] to find
 *	the adjacent scale degree, wrapping across the octave boundary at g/a.
 */
node *fn_turn(ENVIRONMENT *env)
{
    char c;
    int base_note, upper_idx, lower_idx;
    int upper_oct_adj = 0, lower_oct_adj = 0;
    int main_note, upper_note, lower_note;
    double note_dur = env->duration / 4.0;
    double start = env->start;
    int notes[4];
    node *list = NULL;
    node *n;
    int i;

    /* skip any whitespace left after the function name */
    while( isspace(c = nextc()) );

    if( !is_real_note(c) ) {
        parse_error("turn: expected a note name");
        while( (c = nextc()) != END_FUN );
        return NULL;
    }

    base_note = tolower(c) - 'a';   /* 0=a .. 6=g */

    /* get_note_number reads accidentals/octave modifiers from the stream
     * and may update env->octave (if an absolute octave digit is given).
     */
    main_note = get_note_number(tolower(c), env);

    /* Diatonic neighbor octave adjustment: compare raw semitone values.
     * note_table maps letter indices (a=0..g=6) to semitones within octave,
     * but the MIDI octave starts at C (index 2).  So going up from 'b' to
     * 'c' crosses an octave boundary (note_table[c]=0 < note_table[b]=11),
     * and going down from 'c' to 'b' does too.
     */
    int base_value  = note_table[base_note] + env->key[base_note];

    /* upper diatonic neighbor */
    upper_idx   = (base_note + 1) % 7;
    int upper_value = note_table[upper_idx] + env->key[upper_idx];
    upper_oct_adj = (upper_value < base_value) ? 1 : 0;
    upper_note = (env->octave + upper_oct_adj) * 12
               + upper_value
               + TUNING_BIAS + env->transpose;

    /* lower diatonic neighbor */
    lower_idx   = (base_note + 6) % 7;
    int lower_value = note_table[lower_idx] + env->key[lower_idx];
    lower_oct_adj = (lower_value > base_value) ? -1 : 0;
    lower_note = (env->octave + lower_oct_adj) * 12
               + lower_value
               + TUNING_BIAS + env->transpose;

    /* consume closing ')' -- get_note_number pushes back the last char */
    while( isspace(c = nextc()) );
    if( c != END_FUN ) pushc(c);

    /* upper, main, lower, main */
    notes[0] = upper_note;
    notes[1] = main_note;
    notes[2] = lower_note;
    notes[3] = main_note;

    for( i = 0; i < 4; i++ ) {
        n = new_node();
        n->next     = n;
        n->start    = start + i * note_dur;
        n->duration = note_dur;
        n->volume   = env->volume;
        n->duty     = env->duty;
        n->channel  = env->channel;
        n->note     = notes[i];
        n->type     = A_NOTE;
        list = insert_node(list, n);
    }

    return list;
}
