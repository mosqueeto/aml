#include "aml.h" 
	
/**do_entity_parameter
 *	parse top level parameters
 *
 *	 Currently the parameters are:
 *		-TRACE		turn on tracing
 *		-tempo<n>	set the tempo to the given value
 *		-volume<n>	set volume (velocity)
 */
void do_entity_parameter(ENVIRONMENT *env)
{
	char c;
	c = nextc();
	switch (c){
	case 'T': trace = 1-trace;
		  traceinit();
		  break;
	case 't': tempo = get_num(); 
		  if( tempo > 1024 ) tempo = 1024;
		  if( tempo < 8 ) tempo = 8;
		  msec_per_beat = (60.0*1000.0)/tempo;
		  break;
	case 'v': song_volume = get_num();
		  if( song_volume > 127 ) song_volume = 127;
		  else if( song_volume < 8 ) song_volume = 8;
	          break;
	default : pushc(c);
		  do_parameter( env );
		  break;
	}
}


/**do_parameter
 *	do any parameters that are in a sequence or set
 *	syntax is:
 *		-<param_name><param_value>
 *	the dash indicates the beginning of a parameter
 *	<param_name> is a contiguous set of alphabetic characters
 *	<param_value> is a sequence of digits, optionally preceeded
 *		by a "-" or a "+".
 */
void do_parameter(ENVIRONMENT *env)
{
	char param_name[16];
	int i=0;
	enter("do_parameter");
	while( isalpha( param_name[i++] = nextc()) );
	pushc(param_name[--i]);
	param_name[i] = '\0';
	if( strcmp(param_name,"ac") == 0 ) {
		set_key(env);
	}
	else if( strcmp(param_name,"ch") == 0 ) {
		env->channel = get_num();
	}
	else if( strcmp(param_name,"o") == 0 ) {
		env->octave = get_num();
	}
	else {
		parse_error("unknown parameter:");
		printf("<%s>\n",param_name);
	}
	leave("do_parameter");
}

	
