#include	<stdio.h>
#include	<ctype.h>
#include 	<string.h>
#include	"fl.h"

void build_arrays(char c, int write_flag);

int main () 
{
	int c, prevchar = '\0', write_flag = WRITE_NONE;
	
	while ( ( c = getchar() ) != EOF ) 
	{
		/* Case 1 */
		if ( write_flag == WRITE_NONE && isalpha(c) ) {
			write_flag = WRITE_TAG;			
		}
		/* Case 1a, there is no value string, = and ; sit next to each other */
		else if ( c == ENTITY_DELIM && prevchar == PAIR_DELIM ) {
			write_flag = WRITE_NONE;
		}
		/* Case 2 - at the end of reading the tag chars to the array */
		else if ( write_flag == WRITE_TAG && prevchar == PAIR_DELIM ) {
			write_flag = WRITE_VAL;
		}
		/* Case 3 at the end of writing the value to its array */
		else if ( (write_flag == WRITE_VAL && c == ENTITY_DELIM) || (write_flag == WRITE_VAL && c == RECORD_DELIM) ) {
			write_flag = WRITE_NONE;
		}
		/* Case 4, there is no value string, = and ; sit next to each other */
		else if ( c == ENTITY_DELIM && prevchar == PAIR_DELIM ) {
			write_flag = WRITE_NONE;
		}
		/* Case 5 - call to error - WRITE_TAG does not see its proper closing delimiter (PAIR_DELIM) */
		else if ( (write_flag == WRITE_TAG && prevchar == ENTITY_DELIM) || ( write_flag == WRITE_TAG && c == RECORD_DELIM) ) {
					printf("\ncase 4 - Here we call fatal\n");
		}
		prevchar = c;
		build_arrays (c, write_flag);
	}
}


void build_arrays (char c, int write_flag) {
	
	static int i = 0, j = 0;
	static char tag_arr[MAXFLD + 1] = "\0", val_arr[MAXVAL + 1] = "\0";
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
		if ( c == RECORD_DELIM ) {
			//RETURN YES & PASS LINKED LIST TO MAILMERGE
		}
		i = 0; j = 0;
		tag_arr[i] = '\0';
		val_arr[j] = '\0';
		
	}
	
	
}	


