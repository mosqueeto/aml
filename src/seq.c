#include "aml.h"

/**seq
	parse a sequence
 */
node *seq(char c, ENVIRONMENT *env )

{
	int count = 0;
	int check = 0;
	int n = 0;
	int inc = 0;
	int done = FALSE;
	node *np = NULL;
	node *list = NULL;
	ENVIRONMENT local_env;

	enter("seq");

	/*
        Go down sequence, doing each element as it is found, and
        assembling an event list for the .  Since the environment is
        passed to note generating routines to let them know how to
        generate their notes, the values for duration and volume in
        the environment are adjusted, passed to the subsidiary
        routine, and restored after the call.  After the event list is
        finished, re-scan it, adjusting the final volumes and
        durations.
	 */

	if( c != BEGIN_SEQ ) error("seq:  no BEGIN_SEQ\n",NULL);

	local_env = *env;
	local_env.start = 0.0;
	local_env.duration = 1.0;

	while( ! done ) {
		np = NULL;
		while( isspace(c = nextc()) );	/* skip white space */
		if( c == END_SEQ ) {
			done = TRUE;
			break;
		} 
		if( isdigit(c) ) {
			np = dur(c,&n,&local_env);
			if( np == NULL ) {
				if( list == NULL ) { /* a check number */
					check = n;
				}
				continue;
			}
			count += n;
		}
		else if( c == BEGIN_FUN ) {
			/* Functions can consume N events and report back via
			 * fn_element_count, so their inner notes each occupy
			 * one time slot in the outer sequence rather than the
			 * whole call counting as one.
			 */
			fn_element_count = 1;
			np = fun(c, &local_env);
			count += fn_element_count;
			if( np == NULL ) {
				local_env.start = (double)count;
				local_env.duration = 1.0;
				local_env.volume = env->volume;
				continue;
			}
		}
		else {
			np = basic(c,&local_env);
			if( np == NULL ) continue;
			count++;
		}
		/* 
			We just completed one element.  If it had events
			in it, append them to the growing list.
		 */
		local_env.start = (double)count;
		local_env.duration = 1.0;
		local_env.volume = env->volume;
		list = append_list(list,np);
	} /* while */

	if( check && check != count ) {
		parse_error("seq: check doesn't equal count\n");
	}
	/*
		Now adjust the durations and the starts
	 */

	if( !empty(list) ) {
		np = list;
		do {
			np = np->next;
			np->duration = 
				(np->duration/count)*env->duration;
			np->start = 
				(np->start/count)*env->duration 
				 + env->start;
		} while( np != list );
	}
	dmp_list(list);
	leave("do_seq");
	return list;
}
