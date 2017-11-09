/* suspend.c - suspend */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  suspend  -  Suspend a process, placing it in hibernation
 *------------------------------------------------------------------------
 */
syscall	suspend(
	  pid32		pid		/* ID of process to suspend	*/
	)
{	/*------ Timer Code with H/W support */
	uint32 		high = 0, low = 0;
	uint64 		start = 0, end = 0;
	asm volatile ( 	"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
	start = ( ((uint64)high << 32) | low);
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	pri16	prio;			/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)) {
		restore(mask);
		return SYSERR;
	}

	/* Only suspend a process that is current or ready */

	prptr = &proctab[pid];

	prptr->prSystemcallArray[prptr->prSystemcallCount] = 8;
	prptr->prSystemcallCount++;
	if ((prptr->prstate != PR_CURR) && (prptr->prstate != PR_READY)) {
		restore(mask);
		return SYSERR;
	}
	if (prptr->prstate == PR_READY) {
		getitem(pid);		    /* Remove a ready process	*/
					    /*   from the ready list	*/
		prptr->prstate = PR_SUSP;
	} else {
		prptr->prstate = PR_SUSP;   /* Mark the current process	*/
		resched();		    /*   suspended and resched.	*/
	}
	prio = prptr->prprio;
	restore(mask);
	asm volatile ( 	"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
	end = ( ((uint64)high << 32) | low);
	prptr-> prSuspendTime = end - start;
	return prio;
}
