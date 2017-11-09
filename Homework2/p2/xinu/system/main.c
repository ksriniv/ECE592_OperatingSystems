/*  main.c  - main */

#include <xinu.h>

extern qid16 user_queue;
process	main(void)
{

	/* Run the Xinu shell */

	recvclr();
	//kprintf ("Main Process \n");
	pid32 p1 = create_user_proc ( timed_execution, 1024, 100, "FUN_A", 1, 100);
	pid32 p2 = create_user_proc ( timed_execution, 1024, 100, "Fun_B", 1, 100);
	resume (p1);
	resume (p2);
	sleepms(20);
	pid32 p3 = create_user_proc ( timed_execution, 1024, 50, "fun_C", 1, 100);
	resume(p3);
//	user_process_print (user_queue);
	return OK;
    
}
