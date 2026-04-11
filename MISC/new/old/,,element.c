#include "aml.h"


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

/**element
 *	Parse an element. 
 */
node *element( ENVIRONMENT *env)
{
	char c;
	node *result;

	enter("element");
	result = NULL;

	while( isspace(c = nextc()) ) ;
	if( c == 0 ) return EOI;
	if( is_note_name(c) ) {
		return( note(c,env);
	{
	else if( c == BEGIN_SEQ ) {
		return( seq(env) );
	}
	else if( c == BEGIN_SET ) {
		return( set(env) );
	}
	else if( c == BEGIN_FUN ) {
		return( fun(env) );
	}
	else if( isdigit(c) ) {
		return( duration(c,env) );
	}
	else if( (c == ACCENT) || (c == SUBDUE) ) {
		return( accent(c,env) );
	}
	else if( c == BEGIN_PARAM ) {
		return( param(env);
	} 
	else {
		printf("<%c>\n",c);
		parse_error("undefined element");
		return NULL;
	}
	leave("element");
	return result;
}

/**accent
	parse an accent or subdue modifier
 */
node *accent( char c, ENVIRONMENT env )
{
	node * list;
	if( c == ACCENT ) {
		env.volume *= EMPHASIZE;
		list = element(env);
		env.volume /= EMPHASIZE;
	}
	else {
		env.volume *= DEEMPHASIZE
		list = element(env);
		env.volume /= DEEMPHASIZE;
	}
	return list;
}



/**duration
	parse a duration modifier
 */
node *duration(ENVIRONMENT env)
{
	int i;
	node *list;

	pushc(c);
	i = get_num();
	env.duration *= i;
	list = element(env);
	env.duration /= i;
}
	
