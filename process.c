#include	<stdio.h>
#include	<ctype.h>
#include 	<string.h>
#include 	<stdlib.h>
#include	"fl.h"
#include	"ws13.h"

struct arr_builder build_arrays(char c, int write_flag);
int set_write_flag(int c, int prevchar, int write_flag, char ent_delim, char rec_delim);

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

	while ( get_record(tab,data,ent_delim,rec_delim) != NO )/* while more data	*/
	{
		mailmerge( tab, fmt );		/* merge with format	*/
		clear_table(tab);		/* discard data		*/
	}
	free_table(tab); 			/* no memory leaks!	*/
	return(0);			
}
/* END process() */

/**
 *	get_record(symtab_t *tp, FILE *fp)
 *
 *	Purpose: reads the next data record from the stream at *fp
 * 			 stores each fieldname/fieldvalue pair in the storage
 *           table pointed to by the tp argument.
 *	Input:   symtab_t *tp  - symbol table
 *		 	 FILE *fp		- stream from datafile passed by process
 *	Output:  stores each fieldname/fieldvalue pair in tp
 *           returns YES upon succesfully storing a record;
 *		     returns NO on end of file.
 *	Errors:  not reported, function calls fatal() and dies in two situations
 *			 1) a fieldentry has  blank fieldname
 *			 2) a field entry is missing an equals sign
 *	history: 2014-11-06 version 1
 **/

int get_record(symtab_t *tp, FILE *fp, char ent_delim, char rec_delim ) 
{
	int c, prevchar = '\0', write_flag = WRITE_NONE;
	struct arr_builder curr_vals;

	while( ( c = fgetc(fp) ) ) 
	{
		if ( write_flag == WRITE_NONE && c == EOF ) {
			break;
		}
		write_flag = set_write_flag(c, prevchar, write_flag, ent_delim, rec_delim);
		curr_vals = build_arrays(c, write_flag);
		if ( write_flag == WRITE_NONE ) {
			if ( strcmp(curr_vals.tag, "\0") != 0 ) {
				insert( tp, curr_vals.tag, curr_vals.val );
			}
			if ( c == EOF ) {
				if ( prevchar != rec_delim ) {
					insert( tp, SYM_TAB_END_OF_RECORD, SYM_TAB_END_OF_RECORD);
				}
				return NO;
			}
			else if ( c == rec_delim ) {
				insert( tp, SYM_TAB_END_OF_RECORD, SYM_TAB_END_OF_RECORD);
				return YES;
			}
		} 
		prevchar = c;
	}
	return NO;
}
/* END get_record() */

/**
 *	set_write_flag(int c, int prevchar, int write_flag, char ent_delim, char rec_delim)
 *
 *	Purpose: helper method for get_record, implements finite state machine logical controls
 *			 to determine the current write_flag
 *	Input:   int c  - the current char
 *		 	 int prevchar - the previous char in the stream
 *			 int write_flag	- the current write process
 *	 		 char ent_delim - the current entity delimiter (key:value pair delim)
 *			 char rec_delim - the current record delimiter
 *	Output:  returns updated write_flag value to get_record()
 *	
 *	Errors:  not reported.
 *	history: 2014-11-17 version 1
 **/
 int set_write_flag(int c, int prevchar, int write_flag, char ent_delim, char rec_delim) {
 		/* Case 1, no write_status, triggers addition to symtab in get_record */
		if ( write_flag == WRITE_NONE && isalpha(c) ) {
			write_flag = WRITE_TAG;		
		}
		/* Case 2a, there is no value string: '=' and ';' sit next to each other */
		else if ( (c == ent_delim && prevchar == PAIR_DELIM) 
			|| (c == rec_delim && prevchar == PAIR_DELIM ) ) {
			write_flag = WRITE_NONE;
		}
		/* Case 2 - at the end of writing the tag to the array, change flag to WRITE_VAL */
		else if ( write_flag == WRITE_TAG && prevchar == PAIR_DELIM ) {
			write_flag = WRITE_VAL;
		}
		/* Case 3 at the end of writing the value to its array, change flag to WRITE_NONE */
		else if ( (write_flag == WRITE_VAL && c == ent_delim) || (write_flag == WRITE_VAL && c == rec_delim) ) {
			write_flag = WRITE_NONE;
		}
		/* Case 4 - call to error - WRITE_TAG does not see its proper closing delimiter (PAIR_DELIM) */
		/* in English: the WRITE_TAG value does not include '=' */
		else if ( (write_flag == WRITE_TAG && prevchar == ent_delim) 
			|| (write_flag == WRITE_TAG && c == rec_delim) ) {
				fatal("Badly formed data file, no '=' found to close tag", " ");		
		}
		/* Case 4a - WRITE_TAG is set, but more PAIR_DELIMS are seen before the ent_delim or rec_delim */
		/* in English: there is more than one '=' in the tag */
		else if ( write_flag == WRITE_NONE && c == PAIR_DELIM ) {
				fatal("Badly formed data file, more than one '=' found in the tag.", " ");		
		}
		return write_flag;
 }
 /* END set_write_flag() */

/**
 *	build_arrays(char c, int write_flag)
 *
 *	Purpose: helper method for get_record, uses the current c
 *			 and write_status flag to build up arrays for storage in symtab.
 *	Input:   char c  - the current char
 *		 	 int write_flag	-the current write process
 *	Output:  stores tag and value arrays in a struct, returns this 
 *			 to get_record() for further processing.
 *	Errors:  not reported.
 *	history: 2014-11-08 version 1
 **/
struct arr_builder build_arrays(char c, int write_flag) {

	static int i = 0, j = 0;
	static char tag_arr[MAXFLD + 1] = "\0", val_arr[MAXVAL + 1] = "\0";
	struct arr_builder ab;
	
	/* WRITE THE TAG TO ITS ARRAY */
	if ( write_flag == WRITE_TAG && i < MAXFLD ) {
	//if ( ( write_flag == WRITE_TAG || write_flag == FMT_TAG_OPN ) && i < MAXFLD ) {
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
	
	/* WRITE FORMAT TAG TO ITS ARRAY */
	if ( write_flag == WRITE_FMT_OPN && i < MAXFLD ) {
		if ( c != FMT_DELIM ) {
			tag_arr[i++] = c;
		}
		tag_arr[i] = '\0';
	}
	
	/* COPY ARRAYS TO STRUCT */	
	if ( write_flag == WRITE_NONE || ( write_flag == WRITE_FMT_OPN && c == FMT_DELIM ) ) {
			strcpy(ab.tag, tag_arr);
			strcpy(ab.val, val_arr);
			i = 0; j = 0;
			tag_arr[i] = val_arr[j] = '\0';
			return ab;
	}
	strcpy(ab.tag, "\0"); strcpy(ab.val, "\0");
	return ab;
}
/* END build_arrays */

/**
 *	mail_merge (char c, int write_flag)
 *
 *	Purpose: helper method for get_record, uses the current c
 *			 and write_status flag to build up arrays for storage in symtab.
 *	Input:   char c  - the current char
 *		 	 int write_flag	-the current write process
 *	Output:  stores tag and value arrays in a struct, returns this 
 *			 to get_record() for further processing.
 *	Errors:  not reported.
 *	history: 2014-11-08 version 1
 **/
 
 void	mailmerge( symtab_t *tp, FILE *fp) {
	
	int c, write_flag = WRITE_FMT_CLS;
	static char tag_arr[MAXFLD + 1] = "\0", un_tag_arr[MAXFLD + 1] = "\0";
	static int i = 0;
	struct arr_builder curr_fmt_tag;
	
	if ( strcmp(firstword(tp), SYM_TAB_END_OF_RECORD) == 0 && table_len(tp) > 1) {
			while( ( c = fgetc(fp)) != EOF ) {
				if ( c == FMT_DELIM && write_flag == WRITE_FMT_CLS ) { /* opening % is encountered */
					write_flag = WRITE_FMT_OPN;
				}
				else if ( write_flag == WRITE_FMT_OPN ) {
					if ( c == DEFAULT_REC_DELIM ) {
						fatal("Badly formed format file, no closing format tag before end of record", " ");
					}
					if ( c != FMT_DELIM ) { /* ... build up fmt tag array ... */
						//tag_arr[i++] = c;
						curr_fmt_tag = build_arrays(c, write_flag);
					}
					else if ( i == 0 ) { /* ... an escaped percent sign, output % to stdout ... */
						putchar('%');
						write_flag = WRITE_FMT_CLS;
					}
					else if ( strlen(curr_fmt_tag.tag) > 1 ) {
						//tag_arr[i] = '\0'; /* close tag string */
						if ( (in_table(tp, curr_fmt_tag.tag) ) ) { /* tag_arr in table */
							printf("%s", (lookup(tp, curr_fmt_tag.tag)));
						} 
						//else if ( tag_arr[0] == UN_FMT_DELIM ) { /* tag_arr a system var? */
						else if ( curr_fmt_tag.tag[0] == UN_FMT_DELIM ) { /* tag_arr a system var? */
							//strcpy(un_tag_arr, tag_arr+1);
							strcpy(un_tag_arr, curr_fmt_tag.tag+1);
							fflush(stdout);
							table_export(tp);
							system(un_tag_arr);
						} 
						tag_arr[0] = un_tag_arr[0] = '\0'; /* resets for next iter */
						i = 0;
						write_flag = WRITE_FMT_CLS;
					}
				}
				else {
					putchar(c);
				}
			}
			fseek(fp, 0L, 0);
	}
}
/* END mail_merge */