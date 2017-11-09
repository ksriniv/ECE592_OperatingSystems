/* xsh_printprocstk.c - xsh_printprocstk */

#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>


extern void printprocstk (int prio); // File in ../system/printprocstk.c
shellcmd xsh_printprocstk(int nargs, char *args[]) {


	if (nargs == 2)
		printprocstk(atoi(args[1]));

	else
		printf ("Please enter exactly one argument\n");	

	return 0;
}
