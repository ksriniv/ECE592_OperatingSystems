/*  main.c  - main */

#include <xinu.h>

void starve_higherproc (void) {
    while(1) {
      int current_time = clktime  ;
    
      kprintf ("\n This message will be displayed every minute!- Highest Prio ");
      while(clktime - current_time <= 60);
    }
}
process	main(void)
{

	/* Run the Xinu shell */
	print_ready_list();
	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));
        kprintf ("\n\n Going to make a starvation case \n");
        pid32 p1 = create (starve_higherproc, 8192, 1000, "starvation", 0, CONSOLE);
        resume(p1);
	/* Wait for shell to exit and recreate it */
	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
    
}
