#include	<stdio.h>
#include	<stdlib.h>
#include	"fl.h"

/*
 *	formletter program version 1.0
 *
 *	usage: fl format < datafile
 *
 *	data comes from stdin, output goes to stdout
 */

static char	*myname ;		/* used by fatal() */

int main(int ac, char *av[])
{
	FILE *fpfmt;
	FILE *fpdat;
	
	myname = *av;
	char ent_delim = ';', rec_delim = '\n';

	/*
	 * 	check that there is at least one arg: the format file
	 */

	if ( ac == 1 )
		fatal("usage: fl format [datafile..]","");

	/*
	 *	then try to open it
	 */

	if (  (fpfmt = fopen( av[1] , "r")) == NULL )
		fatal("Cannot open format file:", av[1]);

	/*
	 *	in full version, code to handle names of data files
	 *	on the command line will appear here
	 */

	/* ... process data command line ... */
	/*  ... if only 2 args, take input fron stdin ... */
	if ( ac == 2 ) {
		process(fpfmt, stdin, ent_delim, rec_delim);
	} 
	/* ... otherwise, look for flags & file names ...*/
	else {
		for (int i = 2; i < ac; i++ ) {
			//printf("\n%s\n", av[i] );
			if ( av[i][0] == '-' ) {
				switch (av[i][1]) {
					case 'd': ent_delim=av[i][2];
							  continue;
					case 'D':  ent_delim='\n';
							  continue;
					case 'r': rec_delim=av[i][2];
					           continue;	
					default:  fatal("badly formed command line flag", " ");
				}
			}
			else if (  (fpdat = fopen( av[i] , "r")) == NULL ) {
				fatal("Cannot open format file:", av[i]);
			}
			process(fpfmt, fpdat, ent_delim, rec_delim);
			ent_delim = ';', rec_delim = '\n';
		}
	}
	return 0;
}

void fatal(char *s1, char *s2)
/*
 *	fatal error handler
 *	purpose: print error message to stderr then exit
 *	input:   two strings that are printed 
 *	never returns
 */
{
	fprintf(stderr, "%s: %s%s\n",  myname, s1, s2 );
	exit(1);
}
