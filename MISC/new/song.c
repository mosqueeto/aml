#include "aml.h"


/**song
 *	Parse a song
 */
int song(ENVIRONMENT env)
{
	env.start = 0.0;
	do {
		output( element(env) );
		if( nnodes != 0 ) {
			error("Memory leak in nodes -- nnodes != 0\n",NULL);
			return FALSE;
		}
		env.start += 1.0;
	} while( IOflag != EOI );
	return TRUE;
}

/**element
 *	Parse an element. 
 */
node *element( char c,ENVIRONMENT env)
{
	node *result;

	enter("element");
	result = NULL;

	while( isspace(c = nextc()) ) ;
	if( c == 0 ) {
		IOflag = EOI;
		result = NULL;
	}
	else if( is_note_name(c) ) {
		result = note(c,env);
	{
	else if( c == BEGIN_SEQ ) {
		result = seq(c,env);
	}
	else if( c == BEGIN_SET ) {
		result = set(c,env);
	}
	else if( c == BEGIN_FUN ) {
		result = fun(c,env);
	}
	else if( isdigit(c) ) {
		result = duration(c,env);
	}
	else if( (c == ACCENT) || (c == SUBDUE) ) {
		result = accent(c,env);
	}
	else if( c == BEGIN_PARAM ) {
		result = param(c,env);
	} 
	else {
		printf("<%c>\n",c);
		parse_error("undefined element");
		result = NULL;
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
	
