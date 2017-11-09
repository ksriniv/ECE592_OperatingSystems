/* kill.c - kill */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{	
	/*------ Timer Code with H/W support */
	
	uint32 		high = 0, low = 0;
	uint64 		start = 0, end = 0;
	asm volatile ( 	"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
	start = ( ((uint64)high << 32) | low);
	
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	int32	i;			/* Index into descriptors	*/

	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}
	freestk(prptr->prstkbase, prptr->prstklen);

	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* Suicide */
		resched();
	
		prptr->prSystemcallArray[prptr->prSystemcallCount] = 9;
		prptr->prSystemcallCount++;
		
		asm volatile ( 	"RDTSC\n\t"
				"mov %%edx, %0\n\t"
				"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
		end = ( ((uint64)high << 32) | low);
		prptr-> prKillTime = end - start;

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;

		prptr->prSystemcallArray[prptr->prSystemcallCount] = 9;
		prptr->prSystemcallCount++;
		
		asm volatile ( 	"RDTSC\n\t"
				"mov %%edx, %0\n\t"
				"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
		end = ( ((uint64)high << 32) | low);
		prptr-> prKillTime = end - start;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	default:
		

		prptr->prSystemcallArray[prptr->prSystemcallCount] = 9;
		prptr->prSystemcallCount++;
		prptr->prstate = PR_FREE;

		asm volatile ( 	"RDTSC\n\t"
				"mov %%edx, %0\n\t"
				"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
		end = ( ((uint64)high << 32) | low);
		prptr-> prKillTime = end - start;
	
	}

	restore(mask);
	
	
	return OK;
}
