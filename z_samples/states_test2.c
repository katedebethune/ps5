#include	<stdio.h>
#include	<ctype.h>
#include 	<string.h>

#define	RECORD_DELIM '\n'
#define ENTITY_DELIM ';'
#define PAIR_DELIM '='
#define WRITE_NONE 0
#define WRITE_TAG 1
#define WRITE_VAL 2
#define	MAXFLD	40
#define	MAXVAL	120


int main () 
{
	int c;
	int i = 0;
	int prevchar = '\0';
	int write_flag = WRITE_NONE;
	char tag_arr[MAXFLD + 1] = "\0";
	char val_arr[MAXVAL + 1] = "\0";

	
	//int insert( symtab_t *tp,  char str[], char val[] )
	
	while ( ( c = getchar() ) != EOF ) 
	{
		/* Case 1 */
		if ( write_flag == WRITE_NONE && isalpha(c) ) {
			write_flag = WRITE_TAG;	
			//printf("Inside case 1: write_flag = %d, char = %c, prevchar = %c\n", write_flag, c, prevchar);		
		}
		/* Case 2a, there is no value string, = and ; sit next to each other */
		else if ( c == ENTITY_DELIM && prevchar == PAIR_DELIM ) {
			tag_arr[i-1] = '\0';
			printf("tag_arr = %s\n", tag_arr);
			val_arr[0] = '\0';
			printf("val_arr = %s\n", val_arr);
			i = 0;
			write_flag = WRITE_NONE;
		}
		/* Case 2 */
		else if ( write_flag == WRITE_TAG && prevchar == PAIR_DELIM ) {
			tag_arr[i-1] = '\0';
			printf("tag_arr = %s\n", tag_arr);
			i = 0;
			write_flag = WRITE_VAL;
			//printf("Inside case 2: write_flag = %d, char = %c, prevchar = %c\n", write_flag, c, prevchar);

		}
		
		/* Case 3 at the end of writing the value */
		else if ( (write_flag == WRITE_VAL && c == ENTITY_DELIM) || (write_flag == WRITE_VAL && c == RECORD_DELIM) ) {
			val_arr[i] = '\0';
			printf("val_arr = %s\n", val_arr);
			i = 0;
			write_flag = WRITE_NONE;
			
			if ( c == RECORD_DELIM ) {
				printf("\n\nHere is where we call mailmerge on the finished linked list\n\n");	
			}
			//printf("Inside case 3: write_flag = %d, char = %c, prevchar = %c\n", write_flag, c, prevchar);
		}
			
		/* Case 4 - call to error */
		//else if (( write_flag == WRITE_TAG && prevchar == ENTITY_DELIM) ||
		//	 ( write_flag == WRITE_NONE && prevchar == PAIR_DELIM )) {
		else if ( write_flag == WRITE_TAG && prevchar == ENTITY_DELIM ) {
					printf("\ncase 4 - Here we call fatal\n");
		}
		//case 4a - splitting fatal cases for testing
		else if ( write_flag == WRITE_TAG && c == RECORD_DELIM ) {
			printf("\ncase4a - here we call fatal\n");
		}
		prevchar = c;
		//process_char(c, write_flag, tag_array, val_array);
		if ( write_flag == WRITE_TAG && i < MAXFLD ) {
			tag_arr[i] = c;
			i++;
		}
		if ( write_flag == WRITE_VAL && i < MAXVAL ) {
				val_arr[i] = c;
				i++;
		}		
	}
}	
