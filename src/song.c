#include "aml.h"

/**song
 *	Parse a song
 */
int song(ENVIRONMENT song_env)
{
	node *list;
	char c;
	do {	
		list = element(&song_env);	
		output( list );
		while( isspace(c = nextc()) ) ;
		pushc(c);
		song_env.start += 1.0 * (tempo/DEFAULT_TEMPO);
	} while( IOflag != EOF );
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
	int n;
	node *result;

	enter("element");
	result = NULL;

	while( isspace(c = nextc()) ) ;
	if( isdigit(c) ) {
		level++;
		result = dur(c,&n,env);
		level--;
	}
	else if( c == BEGIN_PARAM ) {
		level++;
		param(c,env);
		level--;
	} 
	else if( c == BEGIN_FUN ) {
		level++;
		result = fun(c,env);
		level--;
	}
	else {
		level++;
		result = basic(c,env);
		level--;
	} 
	leave("element");
	return result;
}

/**dur
	parse a dur
*/
node *dur(char c, int *n, ENVIRONMENT *env)
{
	node *list;

	enter("dur");
	pushc(c);
	*n = get_num();
	if( *n <= 0 ) {
		parse_error("dur: illegal number\n");
		return NULL;
	}
	c = nextc();
	if( isspace(c) ) return NULL;
	env->duration *= *n;
	list = basic(c,env);
	env->duration /= *n;
	leave("dur");
	return list;
}
/**dyn
	parse a dynamic
*/
node *dyn(char c, ENVIRONMENT *env)
{
	node *list;
	float mult = 1.0;

	enter("dyn");
	if( c == FORTE ) mult = EMPHASIZE;
	else if( c == PIANO ) mult = DEEMPHASIZE;
	else {
		parse_error("dyn: eh? what happened?");
		return NULL;
	}
	env->volume *= mult;
	c = nextc();
	list = basic(c,env);
	env->volume /= mult;
	leave("dyn");
	return list;
}

/**basic
	parse a basic
*/
node *basic(char c, ENVIRONMENT *env)
{
	node *list;
	enter("basic");
	if( c == BEGIN_SEQ ) {
		list = seq(c,env);
	}
	else if( c == BEGIN_SET ) {
		list = set(c,env);
	}
	else if( c == PIANO || c == FORTE ) {
		list = dyn(c,env);
	}
	else if( c == BEGIN_PARAM ) {
		param(c,env);
		list = NULL;
	}
	else if( c == BEGIN_FUN ) {
		list = fun(c,env);
	}
	else if( c == '.' ) {
		list = dot_note(env);
	}
	else if( is_note_name(c) || c == TIE ) {
		list = note(c,env);
	}
	else {
		parse_error("basic: ");
		return NULL;
	}
	leave("basic");
	return list;
}
