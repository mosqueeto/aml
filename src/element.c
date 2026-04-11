#include "aml.h"

void	do_seq_entity( ENVIRONMENT entity_env);
void	do_set_entity( ENVIRONMENT entity_env);
void	do_entity_parameter(ENVIRONMENT * entity_env);
void	do_note_entity( char base_note_name, ENVIRONMENT * entity_env);

#ifdef ML1
#define STANDALONE
#endif
#ifdef PL
#define STANDALONE
#endif

float accent;

/**song
 *	Parse a song
 */
int song(ENVIRONMENT song_env)
{
	int flag;
	do {
		flag = element(&song_env);
		song_env.start += 1.0;
	} while( flag != EOI );
	if( nnodes != 0 ) {
		error("Memory leak in nodes -- nnodes != 0\n",NULL);
		return FALSE;
	}
	return TRUE;
}

/**octave
	move the default octave up or down
*/
void octave(char c, ENVIRONMENT env)
{
	if( c == OCTAVE_UP ) {
		if( env->octave < 122 ) env->octave += 12;
	}
	else {
		if( env->octave >  24 ) env->octave -= 12;
	}
}

/**element
 *	Parse an element. 
 */
node *element( ENVIRONMENT env)
{
	char c;
	node *result;

	enter("element");
	result = NULL;

	while( isspace(c = nextc()) ) ;
	if( c == 0 ) {
		return EOI;
	}
	else if( isdigit(c) ) {
		return( duration(c,&n,env) );
	}
	else if( c == BEGIN_PARAM ) {
		return( param(c,env);
	}
	else if( c == OCTAVE_UP || c == OCTAVE_DOWN ) {
		return octave(c,env);
	}
	else if( c == BEGIN_FUN )
		return fun(c,env);
	}
	else {
		return( basic(env) );
	} 
	leave("element");
	return result;
}

