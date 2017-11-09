/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;
	/*------ Timer Code with H/W support */
	
	uint32 		high = 0, low = 0;
	uint64 		start = 0, end = 0;
	asm volatile ( 	"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
	start = ( ((uint64)high << 32) | low);
	
	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;

	prptr->prSystemcallArray[prptr->prSystemcallCount] = 6;
	prptr->prSystemcallCount++;
	insert(pid, readylist, prptr->prprio);
	resched();
	
	asm volatile ( 	"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
	end = ( ((uint64)high << 32) | low);
	prptr-> prReadyTime = end - start;
	
	return OK;
}
