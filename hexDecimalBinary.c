/* hexDecimalConversion.c simulates a bitwise logical operations on bit operands

	Programmer: George Mobus
	Date: 3/24/16
	
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>   // needed for toupper function


/* gcc hexDecimalBinary.c h2db
   input to program: ./h2db 0x15
   output: 21 0001 0101
   hex number can be entered in either upper or lower case Aa..Ff
*/

int hex2bin (char h, char * nybble) {
	switch (h) {
		case '0': strcpy (nybble, "0000"); break;
		case '1': strcpy (nybble, "0001"); break;
		case '2': strcpy (nybble, "0010"); break;
		case '3': strcpy (nybble, "0011"); break;
		case '4': strcpy (nybble, "0100"); break;
		case '5': strcpy (nybble, "0101"); break;
		case '6': strcpy (nybble, "0110"); break;
		case '7': strcpy (nybble, "0111"); break;
		case '8': strcpy (nybble, "1000"); break;
		case '9': strcpy (nybble, "1001"); break;
		case 'A': strcpy (nybble, "1010"); break;
		case 'B': strcpy (nybble, "1011"); break;
		case 'C': strcpy (nybble, "1100"); break;
		case 'D': strcpy (nybble, "1101"); break;
		case 'E': strcpy (nybble, "1110"); break;
		case 'F': strcpy (nybble, "1111"); break;
	}
	return 0;
}

/*int main(int argc, char *argv[]) {
	int value;
	char lowOrder, highOrder;
	char binary[9], nybble[5], *strp;
	if (argc < 2) {
		printf ("Invalid input to program\n");
		exit (0);
	}
	value = 
	highOrder = toupper(argv[1][2]);
	lowOrder = toupper(argv[1][3]);
	hex2bin (highOrder,binary);
	hex2bin (lowOrder, nybble);
	strcat(binary, nybble);
	printf ("Value = %d, binary = %s\n", value, binary);
	}*/
