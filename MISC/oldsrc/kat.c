/*
	copyright (c) 1989 by Songbird Software and Kent Crispin
			All rights reserved.
*/

/*
 *  kat -- kent's cat -- a quick hack...
 */
#include <stdio.h>
#include <ctype.h>
#define TRUE  1
#define FALSE 0
#define NULL 0
#define BUFSIZE 512
#define HEADERSIZE 80

char *bp,*cp,*ofname,*ifname;
char cp_buf[BUFSIZE];
FILE *fd_in,*fd_out;
int extract = FALSE;
int formfeed  = 0; /* form feed offset */
int n;
char name[16];

main(argc, argv)
int argc;
char *argv[];

{

	strcpy ( name,*argv );
	ofname = "kat.out";
	bp = cp_buf;
        
	while( (--argc > 0) && ((*++argv)[0] == '-') ) {
	   switch ((*argv)[1]) {
	    case 'o':
		if( argc ) {
		  ++argv; --argc;
		  ofname = *argv;}
                else
		  error("kat: no output file name specified",NULL);
		break;
	    default:
		error("kat: illegal option: %s",*argv);
		break;
	    }
	}
	if( (fd_out=fopen(ofname,"w")) == NULL) {
		error("kat: couldn't create output file: %s\n",ofname);
	}
	while( argc-- > 0 ) {
		cp = *argv;
		ifname = *argv;
		if( strcmp(name,ifname) == 0 ) { argv++; break; }
		if( (fd_in = fopen(ifname,"r")) == NULL ) {
		    error("kat: couldn't open %s\n",ifname);
		    break;
		}
		while((n = fread(bp,sizeof(*bp),BUFSIZE,fd_in)) > 0)
		    if( fwrite(bp,sizeof(*bp),n,fd_out) != n)
			error("kat: write failed",NULL);

		fclose(fd_in);
		argv++;
    }
    exit(0);
}

int error(s1,s2)
char *s1,*s2;

{
  printf(s1,s2);
  printf("\n");
  exit(1);
}
