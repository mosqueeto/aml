#include "aml.h"



/**do_accent
 *	parse out an accent, and return a multiplier for the current
 *	volume.
 */
float do_accent(char c)
{
	if( c=='!' ) return 1.2;
	if( c=='?' ) return 0.8;
}


/**do_relative_duration
 *	do a relative duration, and return the appropriate value
 */
int do_relative_duration(char c)
{
	int i;
	enter("do_relative_duration");
	pushc(c);
	i = get_num();
	leaveint("do_relative_duration",i);
	return i;
}
