#include	<stdio.h>
#include	<ctype.h>
#include 	<string.h>
#include 	<stdlib.h>
#include	"fl.h"
#include	"ws13.h"



/**
 *	process(fmt, data)
 *
 *	Purpose: read from datafile, format and output selected records
 *	Input:   fmt 		- input stream from format file
 *		 data		- stream from datafile
 *	Output:  copied fmt to stdout with insertions
 *	Errors:  not reported, functions call fatal() and die
 *	history: 2012-11-28 added free_table (10q BW)
 **/
int process(FILE *fmt, FILE *data, char ent_delim, char rec_delim)
{
	symtab_t *tab;
	
	if ( (tab = new_table()) == NULL )
		fatal("Cannot create storage object","");
	/* while more data */
	while ( get_record(tab,data,ent_delim,rec_delim) != NO ) {
		mailmerge( tab, fmt );		/* merge with format	*/
		clear_table(tab);		/* discard data		*/
	}
	free_table(tab); 			/* no memory leaks!	*/
	return(0);			
}
/* END process() 14 lines */

/**
 *	get_record(symtab_t *tp, FILE *fp)
 *
 *	Purpose: reads the next data record from the stream at *fp 
 * 		 stores each fieldname/fieldvalue pair in the storage 
 *	         table pointed to by the tp argument.
 *	Input:   symtab_t *tp  - symbol table
 *		 FILE *fp      - stream from datafile passed by process
 *	Output:  stores each fieldname/fieldvalue pair in tp
 *		 returns YES upon succesfully storing a record;
 *		 returns NO on end of file.
 *	Errors:  not reported, function calls fatal() and dies 
 * 		 in two situations
 *		 1) a fieldentry has  blank fieldname
 *		 2) a field entry is missing an equals sign
 *	history: 2014-11-06 version 1
 **/

int get_record(symtab_t *tp, FILE *fp, char ent_delim, char rec_delim ) 
{
	int c, prevchar = '\0', write_flag = WRITE_NONE;
	struct arr_builder curr_vals;

	while( ( c = fgetc(fp) ) ) {
		if ( write_flag == WRITE_NONE && c == EOF ) {
			break;
		}
		write_flag = set_write_flag(c, prevchar,
			 write_flag, ent_delim, rec_delim);
		curr_vals = build_table(c, write_flag);
		if ( write_flag == WRITE_NONE ) {
			/* tag is not an empty string */
			if ( strcmp(curr_vals.tag, "\0") != 0 ) { 
				/* insert into symtab */
				insert( tp, curr_vals.tag, curr_vals.val );
			}
			/* mark end of record in symtab */
			if ( c == rec_delim ) {
				insert( tp, SYM_TAB_END_OF_RECORD, 
						SYM_TAB_END_OF_RECORD);
				return YES;
			}
		} 
		prevchar = c;
	}
	return NO;
}
/* END get_record() 29 lines*/

/**
 *	set_write_flag(int c, int prevchar, int write_flag, char ent_delim,
 *			 char rec_delim)
 *
 *	Purpose: helper method for get_record, implements 
 *		 finite state machine
 *		 logical controls to determine the current write_flag
 *	Input:   int c  - the current char
 *		 int prevchar - the previous char in the stream
 *		 int write_flag	- the current write process
 *	 	 char ent_delim - the current entity delimiter 
 *		 (name:value pair delim)
 *		 char rec_delim - the current record delimiter
 *	Output:  returns updated write_flag value to get_record()
 *	
 *	Errors:  not reported.
 *	history: 2014-11-17 version 1
 **/
 int set_write_flag(int c, int prevchar, int write_flag, char ent_delim, 
			char rec_delim) {
 	/* Case 1, no write_status, add to symtab in get_record */
	if ( write_flag == WRITE_NONE && isalpha(c) ) {
		write_flag = WRITE_TAG;		
	}
	/* Case 2, WRITE_TAG open */
	else if ( prevchar == PAIR_DELIM ) {
		/* no value string: '=' & ';' adjacent */
		if ( c == ent_delim || c == rec_delim ) {
			write_flag = WRITE_NONE;
		}
		/* tag boundary reached: chg to WRITE_VAL */
		if ( write_flag == WRITE_TAG ) {
			write_flag = WRITE_VAL;
		}
	}
	/* Case 3, value boundary reached: chg to WRITE_NONE */
	else if ( write_flag == WRITE_VAL && 
			( c == ent_delim || c == rec_delim) )  {
		write_flag = WRITE_NONE;
	}
	/* Case 4, ERROR - WRITE_TAG does not encounter closing delimiter */
	else if ( write_flag == WRITE_TAG && 
			( prevchar == ent_delim || c == rec_delim ) ) {
		fatal("Badly formed data file, no closing '='", " ");
	}
	/* Case 4a, ERROR -  WRITE_TAG is set, mult PAIR_DELIMS are seen */
	else if ( write_flag == WRITE_NONE && c == PAIR_DELIM ) {
		fatal("Badly formed data file, multiple '=' in tag.", " ");
	}
	return write_flag;
 }
 /* END set_write_flag() 32 lines */

/**
 *	build_table(char c, int write_flag)
 *
 *	Purpose: helper method for get_record, uses the current c
 *		 and write_status flag to build up arrays for storage 
 * 		 in symtab.
 *	Input:   char c  - the current char
 *		 int write_flag	-the current write process
 *	Output:  stores tag and value arrays in a struct, returns this 
 *		 to get_record() for further processing.
 *	Errors:  not reported.
 *	history: 2014-11-08 version 1
 **/
struct arr_builder build_table(char c, int write_flag) {

	static int i = 0, j = 0;
	static char tag_arr[MAXFLD + 1] = "\0", val_arr[MAXVAL + 1] = "\0";
	struct arr_builder ab;
	
	/* WRITE THE TAG TO ITS ARRAY */
	if ( write_flag == WRITE_TAG && i < MAXFLD ) {
			if ( c != PAIR_DELIM ) {
				tag_arr[i++] = c;
			}
	}
	tag_arr[i] = '\0';
	
	/* WRITE THE VALUE TO ITS ARRAY */
	if ( write_flag == WRITE_VAL && j < MAXVAL ) {
				val_arr[j++] = c;
	}
	val_arr[j] = '\0';
	
	/* COPY ARRAYS TO STRUCT */	
	if ( write_flag == WRITE_NONE ) {
			strcpy(ab.tag, tag_arr);
			strcpy(ab.val, val_arr);
			i = j = 0;
			tag_arr[i] = val_arr[j] = '\0';
			return ab;
	}
	strcpy(ab.tag, "\0"); strcpy(ab.val, "\0");
	return ab;
}
/* END build_table  30 lines */

/**
 *	mail_merge (symtab_t *tp, FILE *fp)
 *
 *	Purpose: helper method for process(), prints format text with
 *		 replacement values inserted in the appropriate spots.
 *	Input:   symtab_t *tp - the current symbol table
 * 		 FILE *fp - the file stream for the format file
 *	Output:  format text with replacement values to stdout
 *	Errors:  not reported.
 *	history: 2014-11-08 version 1
 **/
 
void	mailmerge( symtab_t *tp, FILE *fp) {
	
	int c, write_flag = WRITE_FMT_CLS;
	
	if ( strcmp(firstword(tp), SYM_TAB_END_OF_RECORD) == 0 
		&& table_len(tp) > 1) {
		while( ( c = fgetc(fp)) != EOF ) {
			/* opening % is encountered */
			if ( c == FMT_DELIM && write_flag == WRITE_FMT_CLS )
			{ 
				write_flag = WRITE_FMT_OPN;
			}
			else if ( write_flag == WRITE_FMT_OPN ) {
				write_flag = write_fmt_tag(c, 
						write_flag, tp);
			}
			else {
				putchar(c);
			}
		}
		fseek(fp, 0L, 0);
	}
}
/* END mail_merge() 18 lines */

/**
 *	write_fmt_tag(int c, int write_flag, symtab_t *tp) 
 *
 *	Purpose: helper method for mailmerge builds format tags, then writes
 * 		 corresponding values to stdout. 
 *	Input:   char c  - the current char
 *		 int write_flag	-the current write process
 *		 symtab_t *tp - the current symbol table
 *	Output:  maps format tags to data fields and outputs data values to
 * 		 stdout in the appropriate spots. Identifies system 
 *		 variables and outputs these to the appropriate spots.
 *  	Return:  write_flag to the mailmerge process
 *	Errors:  not reported.
 *	history: 2014-11-17 version 1
 **/

int write_fmt_tag(int c, int write_flag, symtab_t *tp) {
	static char tag_arr[MAXFLD + 1] = "\0", 
			un_tag_arr[MAXFLD + 1] = "\0";
	static int i = 0;

	if ( c == DEFAULT_REC_DELIM ) {
			fatal("Badly formed format file", " ");
		}
		if ( c != FMT_DELIM ) {
			tag_arr[i++] = c;
		}
		/* An escaped percent sign, output % to stdout */
		else if ( i == 0 ) {
			putchar(c);
			write_flag = WRITE_FMT_CLS;
		}
		else if ( strlen(tag_arr) > 1 ) {
			tag_arr[i] = '\0'; 
			/* check that tag_arr is in table */
			if ( (in_table(tp, tag_arr) ) ) {
				printf("%s", (lookup(tp, tag_arr)));
			} 
			/* tag_arr a system variable? */
			else if ( tag_arr[0] == UN_FMT_DELIM ) {
				strcpy(un_tag_arr, tag_arr+1);
				fflush(stdout); /* set the environment */
				table_export(tp);
				system(un_tag_arr); /* output sys var */
			} 
			tag_arr[0] = un_tag_arr[0] = '\0';
			i = 0;
			write_flag = WRITE_FMT_CLS;
		}
		return write_flag;
}
/* END write_fmt_tag() 34 lines  */
