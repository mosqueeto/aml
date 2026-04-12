#include "aml.h" 

/**param
	do any parameters that are in a sequence or set

	the syntax is 
		'-' param_name param_value

	<param_name> is a contiguous set of alphabetic characters
	<param_value> is a sequence of digits, optionally preceeded
		by a "-" or a "+".
 */
int duty;

void param(char c, ENVIRONMENT *env)
{
	char param_name[16];
	int i=0;
	enter("param");
	while( isalpha( param_name[i++] = nextc()) );
	pushc(param_name[--i]);
	param_name[i] = '\0';
	if( !strcmp(param_name,"ac") ) { //accidental
		set_key(env);
	}
	else if( !strcmp(param_name,"ch") ) { //channel
		env->channel = get_num();
	}
	else if( !strcmp(param_name,"o") ) { //octave
		env->octave = get_num();
	}
        else if( !strcmp(param_name,"v") ) { //volume (velocity)
                env->volume = (double)get_num();
        }
	else if( !strcmp(param_name,"t") || !strcmp(param_name,"tempo") ) {
        if( level != 1 ) {
                parse_error("tempo must be at top level");
        }
        else {
                tempo = (double)get_num();
                if( tempo > 1024 ) tempo = 1024;
                if( tempo < 8 ) tempo = 8;
                msec_per_beat = (60.0*1000.0)/tempo;
        }
    } 
	else if( !strcmp(param_name,"d") || 
             !strcmp(param_name,"duty") ) { //duty cycle
        if( level != 1 ) {
            parse_error("duty must be at top level");
        }
        else {
            duty = get_num();
            if( duty > 100 ) tempo = 100;
            if( duty < 8 ) duty = 8; 
            song_env.duty = duty;
        }
    } 
	else {
		parse_error("unknown parameter:");
		printf("<%s>\n",param_name);
	}
	leave("param");
}

	
