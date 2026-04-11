#include "aml.h"

node * fn_add();
node * fn_sub();
node * fn_mul();
node * fn_div();
node * fn_def();
node * fn_tempo();
node * fn_volume();

/*	types of the return values of functions */

#define NUM_TYPE	0
#define NODE_TYPE	1
#define NULL_TYPE	3

struct fn_descriptor {
	char *	name;
	int	type;
	void	*(*ex)();
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
	while( !isspace( c = nextc() ) && i < 32 && (c != END_FUNC ) ) {
		current_function[i++] = c;
	}
	current_function[i] = '\0';
}

/**do_function
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
node *do_fun( ENVIRONMENT *env )
{
	int i = 0;
	node * np = NULL;
	enter("do_function");
	get_function_name();
	while( fn_table[i].name != NULL ) {
		if( strcmp( current_function,fn_table[i].name ) == 0 ) {
			np = (void *)fn_table[i].ex();
			break;
		}
		i++;
	}
	leave("do_function");
	return np;
}

node * fn_add()
{
	printf("fn_add called\n");
	return NULL;
}
node * fn_sub()
{
	printf("fn_sub called\n");
	return NULL;
}
node * fn_mul()
{
	printf("fn_mul called\n");
	return NULL;
}
node * fn_div()
{
	printf("fn_div called\n");
	return NULL;
}
node * fn_def()
{
	printf("fn_def called\n");
	return NULL;
}
node * fn_tempo()
{
	printf("fn_tempo called\n");
	return NULL;
}
node * fn_volume()
{
	printf("fn_volume called\n");
	return NULL;
}

/**get_int_arg
 *	Very much like get_num, except that ...
 */
int get_int_arg()
{
}
