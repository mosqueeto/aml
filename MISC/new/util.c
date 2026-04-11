/*	util.c -- various utilities for aml, ami, pl, and so on
 *	
 */

#include "aml.h"

#ifdef AML
#define STANDALONE
#endif
#ifdef PL
#define STANDALONE
#endif

#define TRACEINDENT 3
int traceindent	= 6;
static char tfname[] = "tracefile";

/**error
 *	print an error message and terminate
 */	
void error(char *s1, void *s2 )
{
#ifdef STANDALONE
	fprintf(stderr, "%s: ", program );
	fprintf(stderr, s1, s2 );
	exit(1);	
#else
	mlwrite(s1,s2);
#endif
}
/**traceinit
 *	initialize a trace file
 */
void traceinit()
{
	/*
	if( (tfile == NULL) && (tfile = fopen( tfname,"a" )) == NULL ) {
		error("couldn't open %s\n",tfname);
	}
	*/
}

/**traceclose();
 *	close the trace file if it is open
 */
void traceclose()
{
	/*
 	if( (tfile != NULL) && fclose(tfile) )
 		error("couldn't close the trace file\n",NULL);
 	tfile = NULL;
 	*/
}
 	
/**enter
 *	standard function tracing support
 */
void enter(char *s)
{
	int i;
	if(trace) {
		for( i=0;i<traceindent;i++) fprintf(stderr,"%c",' ');
		fprintf(stderr,"->%s\n", s );
		traceindent += TRACEINDENT;
		traceclose();
		traceinit();
	}
}

/**leave...
 *	several routines that support function tracing
 */
void leave(char *s)
{
	int i;
	if(trace) {
		traceinit();
		traceindent -= TRACEINDENT;
		for( i=0;i<traceindent;i++) fprintf(stderr,"%c",' ');
		fprintf(stderr,"<-%s\n", s );
		traceclose();
	}
}
void leaveint(char *s ,int n)
{
	int i;
	if(trace) {
		traceinit();
		traceindent -= TRACEINDENT;
		for( i=0;i<traceindent;i++) fprintf(stderr,"%c",' ');
		fprintf(stderr,"<-%s: %d\n", s, n );
		traceclose();
	}
}
void leavechar(char *s ,char c)
{
	int i;
	if(trace) {
		traceinit();
		traceindent -= TRACEINDENT;
		for( i=0;i<traceindent;i++) fprintf(stderr,"%c",' ');
		fprintf(stderr,"<-%s: %c\n", s, c );
		traceclose();
	}
}
void leavestring(char *s1 ,char *s2)
{
	int i;
	if(trace) {
		traceinit();
		traceindent -= TRACEINDENT;
		for( i=0;i<traceindent;i++) fprintf(stderr,"%c",' ');
		fprintf(stderr,"<-%s: %s\n", s1, s2 );
		traceclose();
	}
}
void leavefloat(char *s1 ,float f)
{
	int i;
	if(trace) {
		traceinit();
		traceindent -= TRACEINDENT;
		for( i=0;i<traceindent;i++) fprintf(stderr,"%c",' ');
		fprintf(stderr,"<-%s: %f\n", s1, f );
		traceclose();
	}
}
