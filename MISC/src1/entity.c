#include "ml1.h"

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

/**compile
 *	Parse a song
 */
int compile(ENVIRONMENT song_env)
{
	int flag;
	if( setjmp(state_buf) != 0 ) {
		return FALSE;
	}
	do {
		flag = do_entity(&song_env);
	} while( flag == NO_NOTES );
	if( nnodes != 0 ) {
		mlwrite("nnodes != 0");
		return FALSE;
	}
	return TRUE;
}

/**do_entity
 *	Parse an entity.  There are several global data items that are
 *	used by this routine.
 */
int do_entity( ENVIRONMENT *entity_env)
{
	char c;
	int result;
	enter("do_entity");
	result = NO_NOTES;
	while( isspace(c = nextc()) ) ;
	if( c == 0 ) return EOI;
	if( is_note_name(c) ) {
		do_note_entity(c,entity_env);
		result = DID_NOTES;
	}
	else if( isdigit(c) ) {
		entity_env->duration *= do_relative_duration(c);
	}
	else if( c == BEGIN_SET ){
		do_set_entity(*entity_env);
		result = DID_NOTES;
	} 
	else if( c == BEGIN_SEQ ){
		do_sequence_entity(*entity_env);
		result = DID_NOTES;
	} 
	else if( (c == ACCENT) || (c == SUBDUE) ) {
		entity_env->volume *= do_accent(c);
	}
	else if( c == BEGIN_PARAM ) {
		do_entity_parameter(entity_env);
	} 
	else if( c == BEGIN_FUNC ) {
		do_function(entity_env);
	} 
	else {
#ifndef STANDALONE
		printf("<%c>\n",c);
#endif
		parse_error("illegal character in top level parameter");
		return FALSE;
	}
	leave("do_entity");
	return result;
}

/**do_sequence_entity
 *	parse a sequence and output the results
 */
void do_seq_entity(ENVIRONMENT seq_env)
{
	output(do_seq(seq_env));
}


/**do_set_entity
 *	parse a set and output the results
 */
void do_set_entity(ENVIRONMENT set_env)
{
	output(do_set(set_env));
}


/**do_note_entity
 *	Parse a note, and output the result to the output file
 */
void do_note_entity(char base_note_name, ENVIRONMENT *note_env)
{
	output(do_note(base_note_name, note_env));
}
