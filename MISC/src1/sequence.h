/*
 *	file "Sequence.h"	header file for sequence abstraction
 *	Modification History:
 *	89.02.12 Hal - created.
 */

#ifdef NEWDS
/*
 *	NOTE:  Current limitations of this sequence implementation include:
 *	- the sequence will not be grown beyond the allocation size, if needed.
 */
 
/*
 *	newSequence() is called to allocate and intialize a new data
 *	structure.  The caller supplies an estimated number of events,
 *	or zero (0) to specify the default size.
 */
extern SongEventSequence *newSequence( int estNoEvents );

/*
 *	disposeSequence() should be called when done with a data structure
 *	allocated via newSequence().
 */
extern void disposeSequence( SongEventSequence * thisSeq );

/*
 *	sequenceStatus() writes various information to stdout.
 */
extern void sequenceStatus( SongEventSequence * thisSeq );

/*
 *	writeSequence() is used to place the sequence onto the specified
 *	file.  If "file" is NULL, then "fileName" is used.
 */
extern boolean writeSequence( FILE * file,
			      char * fileName,
			      SongEventSequence * thisSeq );

