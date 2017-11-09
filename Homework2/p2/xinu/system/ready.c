/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/
extern qid16 user_queue;
extern uint32 USERPROC;
/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */

status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	
	if (prptr->prprio < USERPROC)
		insert(pid, readylist, prptr->prprio);
	else{
		insert(pid, user_queue, prptr->prruntime);
	}
	resched();

	return OK;
}
