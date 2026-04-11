#include "aml.h"

#ifdef AML
#define STANDALONE
#endif
#ifdef PL
#define STANDALONE
#endif

float accent;

/**do_sequence
 *	parse a sequence
 */
node *do_seq( ENVIRONMENT seq_env )

{
	int initial_count=0, count=0, i=0;
	int done = FALSE;
	int rel_dur = 1;
	node *np = NULL;
	node *list = NULL;
	node *tp;
	float start = 0.0;
	float seq_duration = seq_env.duration;	/*save these values so that */
	float seq_volume = seq_env.volume;	/*they can be restored after*/
						/*a call to some note_produc*/
						/*ing routine*/ 
	char c;
	
	enter("do_seq");

	/*
        Go down sequence, doing each element as it is found, and
        assembling the entire event list.  Since the environment is
        passed to note generating routines to let them know how to
        generate their notes, the values for duration and volume in
        the environment are adjusted, passed to the subsidiary
        routine, and restored after the call.  After the event list is
        finished, re-scan it, adjusting the final volumes and
        durations according to the number of elements in the sequence. 
	 */

	 while( ! done ) {
		if( np != NULL ) rel_dur = 1;
		np = NULL;
printf("count=%d\n",count);
		while( isspace(c = nextc()) );	/* skip white space */
		if( c == END_SEQ ) {
			done = TRUE;
		} 
		else if( is_note_name(c) ) {
			np = do_note(c,&seq_env);
		}
		else if( isdigit(c) ) {
			if( rel_dur != 1 ) {
				free_list(list);
				parse_error("unused duration");
				return NULL;
			}
			rel_dur = do_relative_duration(c);
		}
		else if( c == BEGIN_SET ) {
			np = do_set(seq_env);
		}
		else if( c == BEGIN_SEQ ) {
			np = do_seq(seq_env);
		}
		else if( (c == ACCENT) || (c == SUBDUE) ) {
			accent = do_accent(c);
		}
		else if( c == BEGIN_PARAM ) {
/*			do_parameter(&seq_env);*/
		}
		else if( c == BEGIN_FUNC ) {
			np = do_fun(&seq_env);
		}
		else {
			printf("<%c>\n",c);
			free_list(list);
			parse_error("illegal character in sequence");
			return NULL;
		}
		/* 
		 *	We just completed one element.  If it had events
		 *	in it, append them to the growing list.
		 */
		seq_env.duration = seq_duration * rel_dur;
printf("seq_env.dur = %f,rel_dur=%d\n",seq_env.duration,rel_dur);
		if( np != NULL ) {
			/* we have events, so count them, and reset
			 * default duration and volume
			 */
			tp = np;
			do {
				tp = tp->next;
				tp->start += (count * 1.0);
			} while( tp != np );
			count += rel_dur;
			seq_env.volume = seq_volume;
			list = append_list(list,np);
		}
	}
	/*
	 *	Now figure out the durations and the start times
	 */
	if( !empty(list) ) {
		np = list;
		do {
			np = np->next;
			np->duration /= count;
			np->start /= count;
		} while( np != list );
	}
	dmp_list(list);
	leave("do_seq");
	return list;
}
