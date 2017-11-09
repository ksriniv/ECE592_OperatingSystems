/*  main.c  - main */

#include <xinu.h>

process	main(void)
{

	/* Run the Xinu shell */
        pid32 p1;
	recvclr();
        p1 = create_user_proc (burst_execution, 1024, 10, "burst", 3,2,100,100);
        resume (p1);
	return OK;
    
}
