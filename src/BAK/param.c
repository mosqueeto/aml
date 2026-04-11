#include "aml.h" 

/**param
	do any parameters that are in a sequence or set

	the syntax is 
		'-' param_name param_value

	<param_name> is a contiguous set of alphabetic characters
	<param_value> is a sequence of digits, optionally preceeded
		by a "-" or a "+".
 */
void param(char c, ENVIRONMENT *env)
{
	char param_name[16];
	int i=0;
	enter("param");
	while( isalpha( param_name[i++] = nextc()) );
	pushc(param_name[--i]);
	param_name[i] = '\0';
	if( !strcmp(param_name,"ac") ) {
		set_key(env);
	}
	else if( !strcmp(param_name,"ch") ) {
		env->channel = get_num();
	}
	else if( !strcmp(param_name,"o") ) {
		env->octave = get_num();
	}
        else if( !strcmp(param_name,"v") ) {
                env->volume = (float)get_num();
        }
	else if( !strcmp(param_name,"t") || !strcmp(param_name,"tempo") ) {
                if( level != 1 ) {
                        parse_error("tempo must be at top level");
                }
                else {
                        tempo = (float)get_num();
                        if( tempo > 1024 ) tempo = 1024;
                        if( tempo < 8 ) tempo = 8; 
                        msec_per_beat = (60.0*1000.0)/tempo;
                }
        } 
	else {
		parse_error("unknown parameter:");
		printf("<%s>\n",param_name);
	}
	leave("param");
}

	
