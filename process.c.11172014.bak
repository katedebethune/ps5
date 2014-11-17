#include	<stdio.h>
#include	<ctype.h>
#include 	<string.h>
#include 	<stdlib.h>
#include	"fl.h"
#include	"ws13.h"

int set_write_flag(int c, int prevchar, int write_flag, char ent_delim, char rec_delim);
struct arr_builder build_arrays(char c, int write_flag);

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

	while ( get_record( tab, data, ent_delim, rec_delim ) != NO )/* while more data	*/
	{
		mailmerge( tab, fmt );		/* merge with format	*/
		clear_table(tab);		/* discard data		*/
	}
	free_table(tab); 			/* no memory leaks!	*/
	return(0);			
}

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
		if ( write_flag == WRITE_NONE ) { /* ... add to the list ... */
			if ( strcmp(curr_vals.tag, "\0") != 0 ) { /* ... insert if tag isn't empty ... */
				insert( tp, curr_vals.tag, curr_vals.val );
			}
			if ( c == EOF ) {
				if ( prevchar != rec_delim ) {
					insert( tp, "complete", "complete");
				}
				return NO;
			}
			else if ( c == rec_delim ) {
				insert( tp, "complete", "complete");
				return YES;
			}
		} 
		prevchar = c;
	}
	return NO;
}
/* END get_record */

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
 	if ( write_flag == WRITE_NONE ) { 
			if ( isalpha(c) ) {
				write_flag = WRITE_TAG; /* open WRITE_TAG */
			}
			if ( c == PAIR_DELIM ) { /* no fieldname */
				fatal("Badly formed data file", " ");
			}
		}
		else if ( prevchar == PAIR_DELIM ) {
			if ( c == ent_delim || c == rec_delim ) { /* = & ; are adjacent, check this*/
				write_flag = WRITE_NONE;
			}
			if ( write_flag == WRITE_TAG ) { /* open WRITE_VAL */
				write_flag = WRITE_VAL;
			}
		}
		else if ( write_flag == WRITE_VAL && ( c == ent_delim || c == rec_delim ) ) { /* reset to WRITE_NONE */
			write_flag = WRITE_NONE;
		}
		else if ( write_flag == WRITE_TAG && 
			( prevchar == ent_delim ||  c == rec_delim) ) { /* field is missing = */
				fatal("Badly formed data file", " ");		
		}
		return write_flag;
}
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
	
	if ( write_flag == WRITE_TAG && i < MAXFLD ) { /* ... write tag to array ... */
			if ( c != PAIR_DELIM ) {
				tag_arr[i++] = c;
			}
	}
	tag_arr[i] = '\0';
	if ( write_flag == WRITE_VAL && j < MAXVAL ) { /* ... write val to array ... */
				val_arr[j++] = c;
	}
	val_arr[j] = '\0';
	if ( write_flag == WRITE_NONE ) { /* ... copy arrays & reset function vars ... */	
			strcpy(ab.tag, tag_arr); strcpy(ab.val, val_arr);
			i = j = 0;
			tag_arr[i] = val_arr[j] = '\0';
			return ab;
	}
	strcpy(ab.tag, "\0"); strcpy(ab.val, "\0"); /* ... ???reset struct??? ... */
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
	
	if ( strcmp(firstword(tp),"complete") == 0 && table_len(tp) > 1) {
			while( ( c = fgetc(fp)) != EOF ) {
				if ( c == FMT_DELIM && write_flag == WRITE_FMT_CLS ) { /* % is encountered */
					write_flag = WRITE_FMT_OPN;
				}
				else if ( write_flag == WRITE_FMT_OPN ) {
					if ( c == DEFAULT_REC_DELIM || c == EOF ) { /* at line/file end with no closing fmt tag */
						fatal("Badly formed format file", " ");
					}
					if ( c != FMT_DELIM ) {
						tag_arr[i++] = c;
					}
					else if ( strlen(tag_arr) > 0 ) {
						tag_arr[i] = '\0'; /* close tag string */
						if ( (in_table(tp, tag_arr) ) ) { /* check if tag_arr in table */
							printf("%s", (lookup(tp, tag_arr)));
						} 
						else if ( tag_arr[0] == UN_FMT_DELIM ) { /* tag_arr a system var? */
							strcpy(un_tag_arr, tag_arr+1);
							fflush(stdout);
							table_export(tp);
							system(un_tag_arr);
						} 
					//else if ( strlen(tag_arr) == 0 ) { 
					else { /* ... an escaped % sign, output % to stdout ... */
						putchar(c);
						write_flag = WRITE_FMT_CLS;
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

