#include	<stdio.h>
#include	<ctype.h>

/*
 * Capitalize1.C
 *	Purpose: Filter Data Capitalizing Initials
 * 	Input: Text
 *	Output: Text With Cahrs After '=" or ' ' Changed to Upper Case
 *	Errors: No Error Conditions
 *	Usage: Capitalize < Input
 */

int main()
{
	int	c;
	int	prevchar = '\0';

	while( ( c = getchar() ) != EOF )
	{
		if ( ! isalpha(prevchar) && islower(c) ) {
				c = toupper(c);
		}
		putchar(c);
		prevchar = c;
	}
	return 0;
}
