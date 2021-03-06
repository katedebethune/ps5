#include	<stdio.h>
#include	<ctype.h>
#include 	<string.h>
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
int process(FILE *fmt, FILE *data)
{
	symtab_t *tab;

	if ( (tab = new_table()) == NULL )
		fatal("Cannot create storage object","");

	while ( get_record(tab,data) != NO )/* while more data	*/
	{
		printf("Inside the process while loop\n");
		//mailmerge( tab, fmt );		/* merge with format	*/
		//clear_table(tab);		/* discard data		*/
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

/* int get_record(symtab_t *tp, FILE *fp) 
{
	char fld_nm[MAXFLD + 1] = "\0";
	char val[MAXVAL + 1] = "\0";
	while ( ( c = getchar() ) != EOF ) {
	
		do things
		return YES;
	}
	return NO;
} */

int get_record(symtab_t *tp, FILE *fp) 
{
	int c, prevchar = '\0', write_flag = WRITE_NONE;
	struct arr_builder curr_vals;
	
	/* http://beej.us/guide/bgc/output/html/multipage/getc.html */
	/* while((c = fgetc(fp)) != EOF) {
        if (c == 'b') {
            putchar(c);
        }
    } */
	
	//while ( ( c = getchar() ) != EOF )
	while( ( c = fgetc(fp)) != EOF ) 
	{
		
		/* Case 1 */
		if ( write_flag == WRITE_NONE && isalpha(c) ) {
			write_flag = WRITE_TAG;	
			printf("\ncase 1 write_flag = WRITE_TAG\n");		
		}
		/* Case 2a, there is no value string, = and ; sit next to each other */
		else if ( c == ENTITY_DELIM && prevchar == PAIR_DELIM ) {
			write_flag = WRITE_NONE;
			printf("\ncase 2A write_flag = WRITE_NONE\n");
		}
		/* Case 2 - at the end of writing the tag to the array */
		else if ( write_flag == WRITE_TAG && prevchar == PAIR_DELIM ) {
			write_flag = WRITE_VAL;
			printf("\ncase 2 write_flag = WRITE_VAL\n");
		}
		/* Case 3 at the end of writing the value to its array */
		else if ( (write_flag == WRITE_VAL && c == ENTITY_DELIM) || (write_flag == WRITE_VAL && c == RECORD_DELIM) ) {
			write_flag = WRITE_NONE;
			printf("\ncase 3 write_flag = WRITE_NONE\n");
		}	
		/* Case 4 - call to error - WRITE_TAG does not see its proper closing delimiter (PAIR_DELIM) */
		//else if ( (write_flag == WRITE_TAG && prevchar == ENTITY_DELIM) || ( write_flag == WRITE_TAG && c == RECORD_DELIM) ) {
		else if ( write_flag == WRITE_TAG && prevchar == ENTITY_DELIM ) {
				printf("\ncase 4 - Here we call fatal\n");
				//fatal("Very Badly formed data file", " ");		
		}
		prevchar = c;
		//if ( return build_arrays(c, write_flag) == YES ) {
		//	return YES;
		//}
		curr_vals = build_arrays(c, write_flag);
		if ( write_flag == WRITE_NONE && c != RECORD_DELIM ) {
			printf("\nINSIDE BUILD RECORD else if\n");
			//printf("\ncurr_vals.tag = %s\n", curr_vals.tag);
			//printf("\ncurr_vals.val = %s\n", curr_vals.val);
			//insert( symtab_t *tp,  char str[], char val[] )
			insert( tp,  curr_vals.tag, curr_vals.val );
			show_table(tp);
			break;
		}
		else if ( write_flag == WRITE_NONE && c == RECORD_DELIM ) {
			printf("\nINSIDE COMMIT RECORD else if\n");
			insert( tp,  curr_vals.tag, curr_vals.val );
			show_table(tp);
			//return YES;
		}
	}
	
}

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
	//printf("i = %d, j = %d:: tag_arr = %s, val_arr = %s\n", i,j, tag_arr, val_arr);
	if ( write_flag == WRITE_NONE ) {
		printf("i = %d, j = %d:: tag_arr = %s, val_arr = %s\n", i,j, tag_arr, val_arr);
		//write values to node
		strcpy(ab.tag, tag_arr);
		strcpy(ab.val, val_arr);
		if ( c == RECORD_DELIM ) {
			//RETURN YES & PASS LINKED LIST TO MAILMERGE
		}
		i = 0; j = 0;
		tag_arr[i] = '\0';
		val_arr[j] = '\0';
		return ab;
		
	}
}




