/*  main.c  - main */

#include <xinu.h>

int function_two_para(int a, int b){
	int sum;
	sum=a+b;
//CASE1:AFTER
	printf("\nThe sum of the two parameters %d and %d is: %d\n", a, b, sum);
	return OK;
}

int function_three_para(int a, int b, int c){
	int sum;
	sum=a+b+c;
//CASE2:AFTER
	printf("\nThe sum of the three parameters %d, %d and %d is: %d\n", a, b, c, sum);
	return OK;
}

process	main(void)
{
	//test function with two parameters
//CASE1:BEFORE
	function_two_para(1, 2);
	
	//test function with three parameters
//CASE2:BEFORE
	function_three_para(1, 2, 3);

	/* Run the Xinu shell */
	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
    
}
