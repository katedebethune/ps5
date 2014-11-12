#include 	<stdio.h>
#include	<string.h>

int main(void) {

	char host[100] = "\0";
	strncpy(host, "Kate", 100);
	printf("%s\n", host);
}
