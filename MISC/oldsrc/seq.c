#include "aml.h"

#ifdef AML
#define STANDALONE
#endif
#ifdef PL
#define STANDALONE
#endif

/**do_sequence
 *	parse a sequence
 */
node *do_sequence( ENVIRONMENT seq_env )

{
	int initial_count=0, count=0, i=0;
	int done = FALSE;
	int rel_duration_set = FALSE;	/* this flag is used to see if */
					/* two relative durations in a */
					/* row occur.  This is an error*/
					/* condition. */
	node *np = NULL,*head = NULL,*tail = NULL,*tp;
	float start = 0.0;
	float seq_duration = seq_env.duration;	/*save these values so that */
	float seq_volume = seq_env.volume;	/*they can be restored after*/
						/*a call to some note_produc*/
						/*ing routine*/ 
	char c;
	
	enter("do_sequence");

	/*
	 *Go down sequence, doing each element as it is found, and
	 *assembling the entire event list.  Since the environment
	 *is passed to note generating routines to let them know how
	 *to generate their notes, the values for duration and volume
	 *in the environment are adjusted, passed to the subsidiary
	 *routine, and restored after the call.
	 *After the event list is finished, re-scan it, adjusting the
	 *final volumes and durations according to the number of elements
	 *in the sequence.
	 */
	 	while( ! done ) {
		np = NULL;
		while( isspace(c = nextc()) );	/* skip white space */
		if( c == END_SEQ ) {
			done = TRUE;
		} 
		else if( is_note_name(c) ) {
			np = do_note(c,&seq_env);
			if( head == NULL ) head = np;
			tail->next = np;
			tail = np;
		}
		else if( isdigit(c) ) {
			if( rel_duration_set ) {
				free_node_list(head);
				parse_error("unused duration");
				return NULL;
			}
			i = do_relative_duration(c);
			seq_env.duration *= i;
			count += (i-1);	/* i-1 because of the increment */
		}			/* down a little further for default*/
		else if( c == BEGIN_SET ) {
			np = do_set(seq_env);
			if( head == NULL ) head = np;
			tail->next = np;
			if( np != NULL ) tail = (node *)np->xlink;
		}
		else if( c == BEGIN_SEQ ) {
			np = do_sequence(seq_env);
			if( head == NULL ) head = np;
			tail->next = np;
			if( np != NULL ) tail = (node *)np->xlink;
		}
		else if( (c == ACCENT) || (c == SUBDUE) ) {
			seq_env.volume *= do_accent(c);
		}
		else if( c == BEGIN_PARAM ) {
			do_parameter(&seq_env);
		}
		else if( c == BEGIN_FUNC ) {
			np = do_function(&seq_env);
			if( np != NULL && 
			    (np->type == A_NOTE  || 
			     np->type == NOTE_ON) ) {
				if( head == NULL ) head = np;
				tail->next = np;
				tail = (node *)np->xlink;
			}
		}
		else {
#ifdef STANDALONE
			printf("<%c>\n",c);
#endif
			free_node_list(tail);
			parse_error("illegal character in sequence");
			return NULL;
		}
		/* 
		 *	We just completed one element.  If it had events
		 *	in it, append them to the growing list.
		 */
		if( np != NULL ) {
			/* we have events, so count them, and reset
			 * default duration and volume
			 */
			count++;
			seq_env.duration = seq_duration;
			rel_duration_set = FALSE;
			seq_env.volume = seq_volume;
			if( head == NULL ) {
				head = np;
			}
		}
	}
	/*
	 *	Now figure out the durations and the start times
	 */
	if( tail != NULL ) {
		np = head;
		start = 0.0;
		do {
			np->duration /= count;
			np->start = start;
			start += np->duration;
			np = np->next;
		} while( np != NULL );
	}
	dmp_list(head);
	leave("do_sequence");
	return tail;
}
