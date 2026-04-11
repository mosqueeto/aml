/*
 *	file "Sequence.c"		Defines a Song Event Sequence abstraction
 *	Modification History:
 *	89.02.12 Hal - created.
 */

#include	"ml1.h"
#include	"sequence.h"

#ifdef NEWDS
/*
 *	Local Types
 */
/*
 * Now, the basic type is "subclassed" for the current form of
 * sequencing, an array.  Eventually, this should be hidden from
 * all but the output and playing routines.
 */
#define	DEFAULT_SONG_EVENTS	5000
#define	MAX_SONG_EVENTS	32767
typedef struct {
	SongEventSequence	header;
	int			curSize;	/* elements allocated */
	int			inputIndex;	/* first unused array element */
	int			outputIndex;	/* next array element for get */
	SongEvent		seq[1];		/* size set at declaration */
} SongEventArray;

/*
 *	Define the routines used to access events in the sequence.
 */
static SongEvent * getEvent( SongEventSequence * thisSequencePtr )
{
	SongEvent *nextEvent;
	SongEventArray *seaPtr;
	
	/*
	 * Return the address of the next SongEvent, or NIL (0) if there
	 * isn't one.  NOTE: there is no way to tell if there isn't one
	 * because it hasn't been compiled yet, or because we're done.
	 */
	seaPtr = (SongEventArray *)thisSequencePtr;
	if( seaPtr->outputIndex < seaPtr->inputIndex ) {
		nextEvent = &seaPtr->seq[seaPtr->outputIndex++];
	} else {
		/*
		 * No more events to return.
		 */
		nextEvent = 0;
	}
	return nextEvent;
}

static boolean putEvent( SongEventSequence * thisSequencePtr,
	 		 int event, 
	 		 int note, 
	 		 int velocity,
			 int channel, 
			 int delay )
{
	boolean	result;
	SongEventArray *seaPtr;
	
	/*
	 * Put the information into the next available spot, if there's room.
	 */
	seaPtr = (SongEventArray *)thisSequencePtr;
	if( seaPtr->inputIndex < seaPtr->curSize ) {
		SongEvent *sePtr;
		
		sePtr = &seaPtr->seq[seaPtr->inputIndex++];
		sePtr->event = event;
		sePtr->note  = note;
		sePtr->vel   = velocity;
		sePtr->chan  = channel;
		sePtr->delay = delay;
		result       = true;
	} else {
		error( "SongEventArray overflow at element %d.\n",
				(void *)seaPtr->inputIndex );
		result = false;
	}
	return result;
}


SongEventSequence *newSequence( int estNoEvents )
{
	size_t	allocBytes;
	int	allocElements;
	SongEventArray *sequence;
	
	/*
	 * Calculate the actual size needed for this particular sequence.
	 */
	if( estNoEvents == 0 ) {
		estNoEvents = DEFAULT_SONG_EVENTS;
	}
	allocElements = MIN( estNoEvents, MAX_SONG_EVENTS );
	allocBytes = sizeof( *sequence ) + 
			(allocElements-1) * sizeof( SongEvent );
	sequence = (SongEventArray *)malloc( allocBytes );
	
	/*
	 * If we got the sequence, then initialize it.
	 */
	if( sequence ) {
		sequence->header.getEvent = &getEvent;
		sequence->header.putEvent = &putEvent;
		sequence->curSize = allocElements;
		sequence->inputIndex = 0;
		sequence->outputIndex = 0;
		if( trace ) {
			fprintf(tracefile,"
			  Sequence for %d elements (%u bytes) allocated @%p.\n",
			  allocElements,allocBytes,sequence );
		}
	} else {
		fprintf( tracefile, 
			"not enough memory for %d elements (%u bytes).\n",
			allocElements, allocBytes );
	}
	return (SongEventSequence *)sequence;
}


void disposeSequence( SongEventSequence * thisSeq )
{
	/*
	 * No allocated substructures, so just free the beast.
	 */
	if( thisSeq )  free( thisSeq );
}

/*
 * sequenceStatus() writes various information to stdout.
 */
void sequenceStatus( SongEventSequence * thisSeq )
{
	SongEventArray *seaPtr;
	
	seaPtr = (SongEventArray *)thisSeq;
	printf( "Status of sequence @ %p:\n", thisSeq );
	printf( "\tget routine @ %p, put routine @ %p,\n", thisSeq->getEvent,
		thisSeq->putEvent );
	printf( "\t%5d elements allocated,\n", seaPtr->curSize );
	printf( "\t%5d elements put,\n", seaPtr->inputIndex );
	printf( "\t%5d elements gotten.\n", seaPtr->outputIndex );
}

extern	int	errno;	/* where fopen errors are placed */
/*
 * writeSequence() is used to place the sequence onto the specified
 * file.  If "file" is NULL, then "fileName" is used.
 */
boolean writeSequence(	FILE * file, 
			char * fileName,
			SongEventSequence * thisSeq )
{
	boolean fileSupplied;
	int	oldIndex;	/* used to hold current outputIndex */
	SongEventArray *seaPtr;
	SongEvent *sePtr;
	
	/*
	 * Set up output file.
	 */
	fileSupplied = (file != NULL);
	if( !fileSupplied ) {
		file = fopen( fileName, "w" );
		if( file == NULL ) {
			fprintf( stderr, 
			   "Can't open %s for output, reason %d.\n",
			   fileName, errno );
			exit( 1 );
		}
	}
	
	/*
	 * Output sequence, using it's own access method.  NOTE: we
	 * gunge the structure to save, reset, and restore to existing
	 * output index.  This allows the structure to be written at
	 * any time, irrespective of any other structure access.
	 *
	 * For the future, the better procedure would be to clone the
	 * structure first.
	 */
	seaPtr = (SongEventArray *)thisSeq;
	oldIndex = seaPtr->outputIndex;
	seaPtr->outputIndex = 0;
	while( sePtr = (*thisSeq->getEvent)( thisSeq ) ) {
		fprintf( file, 
			"%4d,%4d,%4d,%4d,%8d\n", sePtr->event, sePtr->note,
			sePtr->vel, sePtr->chan, sePtr->delay );
	}
	seaPtr->outputIndex = oldIndex;
	
	if( !fileSupplied ) {
		fclose( file );		/* clean up our file */
	}
	return ferror( file );
}
