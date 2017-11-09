#include <xinu.h>

uint32 USERPROC = 1000;

struct procent *proc;

extern uint32 ctr1000;

void burst_execution (uint32 number_bursts, uint32 burst_duration, uint32 sleep_duration){
    
    proc = &proctab[currpid];
    uint32 cur_time ;
    while (proc->number_bursts != 0) {
        kprintf ("hello \n"); 
        cur_time = ctr1000;
        while (proc->burst_duration != 0){
            while (ctr1000-cur_time <= 1);
            proc->burst_duration--;
        }
        proc->number_bursts--;
        sleepms (proc->sleep_duration);
      
    }
    kprintf ("P%d-termination: %d", currpid, ctr1000); 
    kill (currpid);

}

pid32 create_user_proc ( void *funcaddr, uint32 ssize, uint32 run_time, char *name, uint32 nargs, uint32 number_bursts, uint32 burst_duration, uint32 sleep_duration, ...){
	


	uint32		savsp, *pushsp;
	intmask 	mask;    	/* Interrupt mask		*/
	pid32		pid;		/* Stores new process id	*/
	struct	procent	*prptr;		/* Pointer to proc. table entry */
	int32		i;
	uint32		*a;		/* Points to list of args	*/
	uint32		*saddr;		/* Stack address		*/

	mask = disable();
	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (uint32) roundmb(ssize);
	if ( ((pid=newpid()) == SYSERR) ||
	     ((saddr = (uint32 *)getstk(ssize)) == (uint32 *)SYSERR) ) {
		restore(mask);
		return SYSERR;
	}
	prcount++;
	prptr = &proctab[pid];

	/* Initialize process table entry for new process */
	prptr->prstate = PR_SUSP;	/* Initial state is suspended	*/
	prptr->prprio = USERPROC;
	prptr->prstkbase = (char *)saddr;
	prptr->prstklen = ssize;

	prptr->prruntime = run_time;
	prptr->prstarttime = ctr1000;
        prptr->number_bursts = number_bursts;
        prptr->burst_duration = burst_duration;
        prptr->sleep_duration = sleep_duration;
        prptr->queuevar = 1;
        prptr->prtimeallotted = TIME_ALLOTMENT;

	prptr->prname[PNMLEN-1] = NULLCH;
	for (i=0 ; i<PNMLEN-1 && (prptr->prname[i]=name[i])!=NULLCH; i++)
		;
	prptr->prsem = -1;
	prptr->prparent = (pid32)getpid();
	prptr->prhasmsg = FALSE;

	/* Set up stdin, stdout, and stderr descriptors for the shell	*/
	prptr->prdesc[0] = CONSOLE;
	prptr->prdesc[1] = CONSOLE;
	prptr->prdesc[2] = CONSOLE;

	/* Initialize stack as if the process was called		*/

	*saddr = STACKMAGIC;
	savsp = (uint32)saddr;

	/* Push arguments */
	a = (uint32 *)(&nargs + 1);	/* Start of args		*/
	a += nargs -1;			/* Last argument		*/
	for ( ; nargs > 0 ; nargs--)	/* Machine dependent; copy args	*/
		*--saddr = *a--;	/* onto created process's stack	*/
	*--saddr = (long)INITRET;	/* Push on return address	*/

	/* The following entries on the stack must match what ctxsw	*/
	/*   expects a saved process state to contain: ret address,	*/
	/*   ebp, interrupt mask, flags, registers, and an old SP	*/

	*--saddr = (long)funcaddr;	/* Make the stack look like it's*/
					/*   half-way through a call to	*/
					/*   ctxsw that "returns" to the*/
					/*   new process		*/
	*--saddr = savsp;		/* This will be register ebp	*/
					/*   for process exit		*/
	savsp = (uint32) saddr;		/* Start of frame for ctxsw	*/
	*--saddr = 0x00000200;		/* New process runs with	*/
					/*   interrupts enabled		*/

	/* Basically, the following emulates an x86 "pushal" instruction*/

	*--saddr = 0;			/* %eax */
	*--saddr = 0;			/* %ecx */
	*--saddr = 0;			/* %edx */
	*--saddr = 0;			/* %ebx */
	*--saddr = 0;			/* %esp; value filled in below	*/
	pushsp = saddr;			/* Remember this location	*/
	*--saddr = savsp;		/* %ebp (while finishing ctxsw)	*/
	*--saddr = 0;			/* %esi */
	*--saddr = 0;			/* %edi */
	*pushsp = (unsigned long) (prptr->prstkptr = (char *)saddr);
	restore(mask);
	
	kprintf ( "P%u - creation : %u\n", pid, prptr->prstarttime);
	return pid;



}

void user_process_print ( qid16 q) {
 qid16 head = firstid (q);
 


 while (queuetab[head].qnext != EMPTY){
	
 kprintf (" Curr is QPrev: %d Key is %d, Qnext is %d\n", queuetab[head].qprev, queuetab[head].qkey, queuetab[head].qnext);
 	head = queuetab[head].qnext;
 }

}






