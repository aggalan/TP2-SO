/* sampleCodeModule.c */
#include "shell.h"

char * v = (char*)0xB8000 + 79 * 2;

static int var1 = 0;
static int var2 = 0;


int main() {
	*v = 'X';
	*(v+1) = 0x74;

//	int * num;
//    scan("%d", num);
//    print("Scanned number: %d\n", *num);
//
//	char * str;
//    scan("%s", str);
//    print("Scanned string: %s\n", str);
//
//	char * c;
//    scan("%c", c);
//    print("Scanned char: %c\n", *c);

	// print("Hello, World!\n");
	// print("This is a %s\n", "test");
	// print("This is test: %d \n", 3);
	// print("This is test: %c\n", '4');


	shellStart();
	//Test if BSS is properly set up
	if (var1 == 0 && var2 == 0)
		return 0xDEADC0DE;

	return 0xDEADBEEF;
}
