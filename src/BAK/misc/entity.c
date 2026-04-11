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

/**compile
 *	Parse a song
 */
int compile(ENVIRONMENT song_env)
{
	int flag;
	do {
		flag = do_entity(&song_env);
	} while( flag != EOI );
	if( nnodes != 0 ) {
		error("Memory leak in nodes -- nnodes != 0\n",NULL);
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
	float dur;
	enter("do_entity");
	result = NO_NOTES;
	while( isspace(c = nextc()) ) ;
	if( c == 0 ) return EOI;
	if( is_note_name(c) ) {
		do_note_entity(c,entity_env);
		master_start += entity_env->duration;
		result = DID_NOTES;
	}
	else if( isdigit(c) ) {
		dur = entity_env->duration;
		entity_env->duration *= do_relative_duration(c);
		result = do_entity(entity_env);
		entity_env->duration = dur;
	}
	else if( c == BEGIN_SET ){
		do_set_entity(*entity_env);
		master_start += entity_env->duration;
		result = DID_NOTES;
	} 
	else if( c == BEGIN_SEQ ){
		do_seq_entity(*entity_env);
		master_start += entity_env->duration;
		result = DID_NOTES;
	} 
	else if( (c == ACCENT) || (c == SUBDUE) ) {
		accent = do_accent(c);
		if( (entity_env->volume * accent) > 128.0 ) 
			entity_env->volume = 128.0;
		else
			entity_env->volume *= accent;
	}
	else if( c == BEGIN_PARAM ) {
		do_entity_parameter(entity_env);
	} 
	else if( c == BEGIN_FUNC ) {
		do_fun(entity_env);
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

/**do_seq_entity
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
