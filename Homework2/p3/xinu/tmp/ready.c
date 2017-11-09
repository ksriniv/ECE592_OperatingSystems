/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/
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
        if (prptr->queuevar == 0){
	    insert(pid, readylist, prptr->prprio);
        }
        else if (prptr->queuevar == 1){
            insert(pid, userqueue1, prptr->prprio);
        }
        else if (prptr->queuevar == 2){
            insert(pid, userqueue2, prptr->prprio);
        }
        else if (prptr-> queuevar == 3){
            insert(pid, userqueue3, prptr->prprio);
        }
        else if (prptr-> queuevar == 4){
            insert(pid, userqueue4, prptr->prprio);
        }

	resched();

	return OK;
}
