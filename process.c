#include	<stdio.h>
#include	<ctype.h>
#include 	<string.h>
#include 	<stdlib.h>
#include	"fl.h"
#include	"ws13.h"

struct arr_builder build_arrays(char c, int write_flag);
//int build_list(struct arr_builder curr_vals, int write_flag, char c, char prevchar) 

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
	
	//ENTITY_DEL('ent_delim');
	//RECORD_DEL('rec_delim');

	if ( (tab = new_table()) == NULL )
		fatal("Cannot create storage object","");

	while ( get_record(tab,data,ent_delim,rec_delim) != NO )/* while more data	*/
	{
		//printf("Inside the process while loop\n");
		mailmerge( tab, fmt );		/* merge with format	*/
		clear_table(tab);		/* discard data		*/
	}

	free_table(tab);			/* no memory leaks!	*/
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
		/* Case 1, no write_status */
		if ( write_flag == WRITE_NONE && isalpha(c) ) {
			write_flag = WRITE_TAG;		
		}
		/* Case 2a, there is no value string, = and ; sit next to each other */
		//else if ( c == ENTITY_DELIM && prevchar == PAIR_DELIM ) {
		else if ( c == ent_delim && prevchar == PAIR_DELIM ) {
			write_flag = WRITE_NONE;
		}
		/* Case 2 - at the end of writing the tag to the array, change flag to WRITE_VAL */
		else if ( write_flag == WRITE_TAG && c == PAIR_DELIM ) {
			write_flag = WRITE_VAL;
		}
		/* Case 3 at the end of writing the value to its array, change flag to WRITE_NONE */
		//else if ( (write_flag == WRITE_VAL && c == ENTITY_DELIM) || (write_flag == WRITE_VAL && c == RECORD_DELIM) ) {
		else if ( (write_flag == WRITE_VAL && c == ent_delim) || (write_flag == WRITE_VAL && c == rec_delim) ) {
			write_flag = WRITE_NONE;
		}
		/* Case 4 - at EOF, break out of this loop */
		else if ( write_flag == WRITE_NONE && c == EOF ) {
			printf("\nINSEIDE FLAG SETTER 2 FOR EOF, C = %c\n", c);
			break;
		}
		/* Case 4 - call to error - WRITE_TAG does not see its proper closing delimiter (PAIR_DELIM) */
		//else if ( write_flag == WRITE_TAG && prevchar == ENTITY_DELIM ) {
		else if ( write_flag == WRITE_TAG && prevchar == ent_delim ) {
				fatal("Badly formed data file", " ");		
		}
		curr_vals = build_arrays(c, write_flag);
		//return build_list(curr_vals, write_flag, c, prevchar); 
		if ( write_flag == WRITE_NONE ) {
			if ( strcmp(curr_vals.tag, "\0") != 0 ) {
				insert( tp, curr_vals.tag, curr_vals.val );
			}
			if ( c == EOF ) {
				//if ( prevchar != RECORD_DELIM ) {
				if ( prevchar != rec_delim ) {
					insert( tp, "complete", "complete");
				}
				//printf ("c = %c; prevchar = %c\n", c, prevchar);
				//printf("\nInside EOF && c == RECORD_DELIM, set stop flag loop\n");
				return NO;
			}
			//else if ( c == RECORD_DELIM )
			else if ( c == rec_delim ) {
				insert( tp, "complete", "complete");
				//printf("\nInside RECORD_DELIM loop\n");
				//printf ("c = %c; prevchar = %c\n", c, prevchar);
				return YES;
			}
		} 
		prevchar = c;
	}
	return NO;
}
/* END get_record */

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
//int build_arrays (char c, int write_flag) {
	
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
			if ( c != PAIR_DELIM ) {
				val_arr[j++] = c;
			}	
	}
	val_arr[j] = '\0';
	/* WRITE FORMAT TAG TO ITS ARRAY */
	if ( write_flag == WRITE_FMT_OPN && i < MAXFLD ) {
		if ( c != FMT_DELIM ) {
			tag_arr[i++] = c;
		}
	}
	tag_arr[i] = '\0';
	/* COPY ARRAYS TO STRUCT */	
	if ( write_flag == WRITE_NONE ) {
			strcpy(ab.tag, tag_arr);
			strcpy(ab.val, val_arr);
			i = 0; j = 0;
			tag_arr[i] = '\0';
			val_arr[j] = '\0';
			return ab;
	}
}
/* END build_arrays */

/**
 *	build_list(struct arr_builder curr_vals, int write_flag, char c, char prevchar)
 *  FIX EXPLANATIONS
 *	Purpose: helper method for get_record, uses the current c
 *			 and write_status flag to build up arrays for storage in symtab.
 *	Input:   char c  - the current char
 *		 	 int write_flag	-the current write process
 *	Output:  stores tag and value arrays in a struct, returns this 
 *			 to get_record() for further processing.
 *	Errors:  not reported.
 *	history: 2014-11-12 version 1
 **/

/* build_list(struct arr_builder curr_vals, int write_flag, char c, char prevchar) {

if ( write_flag == WRITE_NONE ) {
		if ( strcmp(curr_vals.tag, "\0") != 0 ) {
			insert( tp, curr_vals.tag, curr_vals.val );
		}
		if ( c == EOF ) {
			if ( prevchar != RECORD_DELIM ) {
				insert( tp, "complete", "complete");
			}
			printf("\nInside EOF && c == RECORD_DELIM, set stop flag loop\n");
			return NO;
		}
		else if ( c == RECORD_DELIM ) {
			insert( tp, "complete", "complete");
			//printf("\nInside RECORD_DELIM loop\n");
			//printf ("c = %c; prevchar = %c\n", c, prevchar);
			return YES;
		}
	}
} */

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
	
	int c, curr_tab, write_flag = WRITE_FMT_CLS;
	static char tag_arr[MAXFLD + 1] = "\0", val_arr[MAXVAL + 1] = "\0";
	static int i = 0, j = 0;
	//struct arr_builder curr_fmt_vals;
	//struct link curr_tab;
	
	//show_table(tp);
	if ( strcmp(firstword(tp),"complete") == 0 && table_len(tp) > 1) {
			while( ( c = fgetc(fp)) != EOF ) {
				/* write_flag set to WRITE_NONE
				and a % is encountered, set write_flag to WRITE_FMT */
				if ( c == FMT_DELIM && write_flag == WRITE_FMT_CLS ) {
					write_flag = WRITE_FMT_OPN;
				}
				else if ( write_flag == WRITE_FMT_OPN ) {
					//printf("\nInside WRITE_FMT\n");
					if ( c != FMT_DELIM ) {
						tag_arr[i++] = c;
					}
					else if ( i == 0 && c == FMT_DELIM ) {
						putchar('%');
						write_flag = WRITE_FMT_CLS;
					}	
					else if ( strlen(tag_arr) > 1 ) {
						tag_arr[i] = '\0'; /* close tag string */
						curr_tab = in_table(tp, tag_arr);
						/* ... current tag_arr is in the table, print its value to stdout ... */
						if ( curr_tab ) {
							printf("%s", (lookup(tp, tag_arr)));
							write_flag = WRITE_FMT_CLS;
						} 
						/* ... tag_arr not in sym_tab, check to see if it is a system tag ... */
						else if ( tag_arr[0] == UN_FMT_DELIM ) {
							printf("Unix tag");
							write_flag = WRITE_FMT_CLS;
							//continue;
						} 
						/* ... tag_arr not in sym_tab & not a system var ... */
						else if ( !curr_tab ) {
							write_flag = WRITE_FMT_CLS;
						}
						/* ... reset tag_arr ... */
						tag_arr[0] = val_arr[0] = '\0';
						i = 0;
					}
					
				} 
				else {
					putchar(c);
				}
				
			}
			fseek(fp, 0L, 0);
	}
	else {
			//printf("\nMailmerge did not find complete\n");
			;
	}
	//exit(1);
}

