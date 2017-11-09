/*  main.c  - main */

#include <xinu.h>
unsigned long *esp, *before_esp, *after_esp;
unsigned long *ebp, *before_ebp, *after_ebp;
int printCount = 0;
unsigned int i = 0;

extern int pr_status_syscall_summary(void);
/*--------------- Two Parameter Function --------------- */
int function_two_para(int a, int b){
	printCount = 0;
	int sum;
	sum=a+b;
//CASE1:AFTER
	
	asm("movl %%esp, %0\n":"=r"(after_esp));
	asm("movl %%ebp, %0\n":"=r"(after_ebp));

	for ( i = 0; i < 12; i++) {
		printCount++;
		if (*(after_esp+i) == STACKMAGIC)
			break;
	}

	kprintf ("\n\nStack pointer : %08x\n", before_esp);
	kprintf ("Last %d elements on the stack \n", printCount);
	kprintf ("address\t\tvalue\n");
	kprintf ("--------\t\t------\n");
	for (i = 0; i < printCount ; i++)
		kprintf ("%08x\t\t%d\n",*(after_esp+i),*(after_esp+i));
	
	printf("\nThe sum of the two parameters %d and %d is: %d\n", a, b, sum);
	return OK;
}

/*--------------- Three parameter Function ------------------------ */
int function_three_para(int a, int b, int c){
	printCount = 0;
	int sum;
	sum=a+b+c;
//CASE2:AFTER
	
	asm("movl %%esp, %0\n":"=r"(after_esp));
	asm("movl %%ebp, %0\n":"=r"(after_ebp));

	for ( i = 0; i < 12; i++) {
		printCount++;
		if (*(after_esp+i) == STACKMAGIC)
			break;
	}	

	kprintf ("\n\nStack pointer : %08x\n", before_esp);
	kprintf ("Last %d elements on the stack \n", printCount);
	kprintf ("address\t\tvalue\n");
	kprintf ("--------\t\t------\n");
	for (i = 0; i < printCount; i++)
		kprintf ("%08x\t\t%d\n",*(after_esp+i),*(after_esp+i));
	printf("\nThe sum of the three parameters %d, %d and %d is: %d\n", a, b, c, sum);
	return OK;
}


/*----------------------- Main Process ------------------------*/
process	main(void)
{	unsigned int i = 0;
	//test function with two parameters
//CASE1:BEFORE
	
	//asm("movl %%ebp, %0\n":"=r"(ebp));
	//asm("movl %%esp, %0\n":"=r"(esp));
	//init_esp  = esp;
	//init_ebp  = ebp;
	
	//kprintf (" The initial Pointers are %08x and %08x\n", init_esp, init_ebp);	
	asm("movl %%esp, %0\n":"=r"(before_esp));
	asm("movl %%ebp, %0\n":"=r"(before_ebp));
	
	for ( i = 0; i < 12; i++) {
		printCount++;
		if (*(before_esp + i ) == STACKMAGIC)
			break;
		
	}

	kprintf ("\n\nStack pointer : %08x\n", before_esp);
	kprintf ("Last %d elements on the stack \n", printCount);
	kprintf ("address\t\tvalue\n");
	kprintf ("--------\t\t------\n");
	for (i = 0; i< printCount ; i++)
		kprintf ("%08x\t\t%d\n",*(before_esp+i),*(before_esp+i));
	
	function_two_para(1, 2);
	printCount = 0;

	//test function with three parameters
//CASE2:BEFORE
	
	asm("movl %%esp, %0\n":"=r"(before_esp));
	asm("movl %%ebp, %0\n":"=r"(before_ebp));
	
	for ( i = 0; i < 12; i++) {
		printCount++;
		if (*(before_esp+i) == STACKMAGIC)
			break;
	}	

	kprintf ("\n\nStack pointer : %08x\n", before_esp);
	kprintf ("Last %d elements on the stack \n", printCount);
	kprintf ("address\t\tvalue\n");
	kprintf ("--------\t\t------\n");

	for (i = 0; i < printCount ; i++)
		kprintf ("%08x\t\t%d\n",*(before_esp+i),*(before_esp+i));
	function_three_para(1, 2, 3);
	//sleep(1);
	//sleep (1);	
	//pr_status_syscall_summary();
	/* Run the Xinu shell */
	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));
	/* Wait for shell to exit and recreate it */
	pr_status_syscall_summary();
	
	kprintf("\n Press Enter to use shell commands \n");
	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
    
}
