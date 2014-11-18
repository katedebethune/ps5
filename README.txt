CSCI e26
Kate de Bethune
Problem set 5
November 17, 2014

/*
 * FILES FOR SUBMISSION
 */

directory: ~/debethune/ps5
/* ...PROGRAM FILES ... */
1. process.c
2. wordstore13.c
3. fl.c
4. fl.h
5. ws13.h

/* ...TEST FILES ... */
1. sample.fmt - class example with additional test for % function
1a. sample.dat - class example altered with test cases & junk. Fails around Luke the cat, due to incorrectly formed tags.
2. sample8.dat - has \n as entity delim and # as record delim (-D -r#)
3. sample6.dat - has @ as entity delim and # as record delim (-d@ -r#)
4. sample1.fmt - includes additional system variable & college major field name
5. sample10.dat - data for use with sample1.fmt

/* ...SUBMISSION FILES ... */
1. Makefile
2. typescript1 - sample run with SKD test data
3. typescript2- sample run with lib113 test data
4. typescript3 - program run with test.fl
5. typescript4 - program run with gcc -Wall ( & Makefile )
6. typescript_skd_files - printout of my formats and data files.

/*
 * ASSIGNMENT REQUIREMENTS
 */

1. Add get_record, mailmerge, and clear_table functions 
	DONE

2. Modify main (in fl.c) to handle mutliple files & command line variants

	DONE - please note that my main() has a line that resets the delim
		flags to the defaults after the file to which they apply has
		been processed. This could easily be switched or removed.


3. Include system variables
	DONE

4. Create a makefile
	DONE


Thank you for grading my assignment.

Kate













































