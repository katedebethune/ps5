#include	<stdio.h>
#include	<ctype.h>
#include 	<string.h>
#include 	<stdlib.h>
#include	"fl.h"
#include	"ws13.h"

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
	printf("\nent_delim = %c\n", ent_delim);
	printf("\nrec_delim = %c\n", rec_delim);
	
	printf("\nINSIDE PROCESS\n");
	printf("\nENTITY_DELIM = %c \n", ENTITY_DELIM);
	printf("\nRECORD_DELIM = %c \n", RECORD_DELIM);
	
	ENTITY_DEL('#');
	RECORD_DEL('$');
	
	ENTITY_DEL('ent_delim');
	RECORD_DEL('rec_delim');

	
	printf("\nENTITY_DELIM = %c \n", ENTITY_DELIM);
	printf("\nRECORD_DELIM = %c \n", RECORD_DELIM);
	
	
	if ( (tab = new_table()) == NULL )
		fatal("Cannot create storage object","");

	while ( get_record(tab,data) != NO )/* while more data	*/
	{
		//printf("Inside the process while loop\n");
		mailmerge( tab, fmt );		/* merge with format	*/
		clear_table(tab);		/* discard data		*/
	}

	//free_table(tab);			/* no memory leaks!	*/
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

int get_record(symtab_t *tp, FILE *fp) 
{
	int c, prevchar = '\0', write_flag = WRITE_NONE;
	struct arr_builder curr_vals;

	//while( ( c = fgetc(fp)) != EOF ) 
	while( ( c = fgetc(fp) ) ) 
	{
		//printf ("c = %c prevchar = %c\n", c, prevchar);
		/* Case 1 */
		if ( write_flag == WRITE_NONE && isalpha(c) ) {
			write_flag = WRITE_TAG;		
		}
		/* Case 2a, there is no value string, = and ; sit next to each other */
		else if ( c == ENTITY_DELIM && prevchar == PAIR_DELIM ) {
			write_flag = WRITE_NONE;
		}
		/* Case 2 - at the end of writing the tag to the array */
		else if ( write_flag == WRITE_TAG && prevchar == PAIR_DELIM ) {
			write_flag = WRITE_VAL;
		}
		/* Case 3 at the end of writing the value to its array */
		else if ( (write_flag == WRITE_VAL && c == ENTITY_DELIM) || (write_flag == WRITE_VAL && c == RECORD_DELIM) ) {
			write_flag = WRITE_NONE;
		}
		else if ( write_flag == WRITE_NONE && c == EOF ) {
			printf("\nINSEIDE FLAG SETTER 2 FOR EOF, C = %c\n", c);
			break;
		}
		else if ( write_flag == WRITE_VAL && c == EOF )  {
			printf("\nINSIDE FLAG SETTER FOR EOF, c = %c\n", c);
			write_flag = WRITE_NONE;
		}
		
		/* Case 4 - call to error - WRITE_TAG does not see its proper closing delimiter (PAIR_DELIM) */
		//else if ( (write_flag == WRITE_TAG && prevchar == ENTITY_DELIM) || ( write_flag == WRITE_TAG && c == RECORD_DELIM) ) {
		else if ( write_flag == WRITE_TAG && prevchar == ENTITY_DELIM ) {
				fatal("Badly formed data file", " ");		
		}
		//prevchar = c;
		//printf ("c = %c; prevchar = %c\n", c, prevchar);
		curr_vals = build_arrays(c, write_flag);
		if ( write_flag == WRITE_NONE ) {
			if ( strcmp(curr_vals.tag, "\0") != 0 ) {
				insert( tp, curr_vals.tag, curr_vals.val );
			}
			if ( c == EOF ) {
				if ( prevchar != RECORD_DELIM ) {
					insert( tp, "complete", "complete");
				}
				//printf ("c = %c; prevchar = %c\n", c, prevchar);
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
	//else if ( write_flag == WRITE_VAL && j < MAXVAL ) {
	if ( write_flag == WRITE_VAL && j < MAXVAL ) {
			val_arr[j++] = c;
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
		//printf("i = %d, j = %d:: tag_arr = %s, val_arr = %s\n", i,j, tag_arr, val_arr);
		//write values to node
		//if ( strlen(tag_arr) > 0 ) {
			strcpy(ab.tag, tag_arr);
			strcpy(ab.val, val_arr);
			i = 0; j = 0;
			tag_arr[i] = '\0';
			val_arr[j] = '\0';
			return ab;
		//} 
		//else {
		//	return NULL;
		//}
	}
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
	static char tag_arr[MAXFLD + 1] = "\0", val_arr[MAXVAL + 1] = "\0";
	static int i = 0, j = 0;
	struct arr_builder curr_fmt_vals;
	//struct link curr_tab;
	int curr_tab;
	
	show_table(tp);
	if ( strcmp(firstword(tp),"complete") == 0 && table_len(tp) > 1) {
			//printf("\nMailmerge found the complete marker\n");
			//show_table(tp);
			while( ( c = fgetc(fp)) != EOF ) {
				//putchar(c);
				/* write_flag set to WRITE_NONE
				and a % is encountered, set write_flag to WRITE_FMT */
				if ( c == FMT_DELIM && write_flag == WRITE_FMT_CLS ) {
					write_flag = WRITE_FMT_OPN;
					//printf("\nINSIDE write_flag = WRITE_FMT\n");
					//continue;
				}
				else if ( write_flag == WRITE_FMT_OPN ) {
					//printf("\nInside WRITE_FMT\n");
					if ( c != FMT_DELIM ) {
						tag_arr[i++] = c;
						//printf("\ni = %d\n", i);
					}
					else if ( i == 0 && c == FMT_DELIM ) {
						putchar('%');
						write_flag = WRITE_FMT_CLS;
					}	
					else if ( strlen(tag_arr) > 1 && c == FMT_DELIM ) {
						tag_arr[i] = '\0';
						curr_tab = in_table(tp, tag_arr);
						if ( curr_tab ) {
							printf("%s", (lookup(tp, tag_arr)));
							write_flag = WRITE_FMT_CLS;
						} 
						else if ( tag_arr[0] == UN_FMT_DELIM ) {
							printf("Unix tag");
							write_flag = WRITE_FMT_CLS;
							//continue;
						} 
						else if ( !curr_tab ) {
							write_flag = WRITE_FMT_CLS;
						}
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
			printf("\nMailmerge did not find complete\n");
			//error out here
	}
	//exit(1);
}

/*
else if  ( c == FMT_DELIM ) {
						//strcpy(tag_arr,curr_fmt_vals.tag);
						//printf("\n%s", curr_fmt_vals.tag);
						//printf("\n%s", tag_arr);
						//printf("\nINSIDE c == FMT_DELIM\n");
						//printf("\nfmt_arr = %s\n", "firstname");
						//curr_tab = in_table(tp, tag_arr);
						//strcpy(val_arr, (lookup(tp, "firstname")));
						//printf("\nfmt_arr = %s\n", curr_tab->val);
						//printf("\nfmt_arr = %d\n", curr_tab);
						//printf("\ntag_value = %s\n", val_arr);
						write_flag = WRITE_NONE;
						i = 0;
						break;
					}
					
					
				if ( write_flag == WRITE_FMT ) {
					
					if ( c == FMT_DELIM && i == 1 ) {
						printf("\nINSIDE c == %%\n");
						//putchar(c);
						write_flag = WRITE_NONE;
						i++;
					}
					if ( c == FMT_DELIM && i == 2 ) {
						if ( i < MAXFLD  && c != FMT_DELIM ) {
							printf("\nINSIDE building array \n");
							i++;
							tag_arr[j++] = c;
						}
						
						else if  ( c == FMT_DELIM ) {
							tag_arr[++j] = '\0';
							printf("\n%s\n", tag_arr);
							write_flag = WRITE_NONE;
							i = 0;
							break;
						}
					}
*/

