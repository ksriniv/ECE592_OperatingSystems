#include <xinu.h>

/* --- Initialize prSyscallCount and prSyscallArray for all processes ---- */


int pr_status_syscall_summary() {
	struct procent *pr;
	int i = 0, j = 0;
	
	int printArray[10] = { 0,0,0,0,0,0,0,0,0,0 };
	kprintf ("\n");	
	kprintf ("pid\tsyscall\t\tcount\t\taverage time in cycles\n ");
	kprintf ("---\t-------\t\t-----\t\t----------------------\n ");
	for ( i = 0; i < NPROC; i++) {
			for ( j = 0; j<10; j++)	
				printArray[j] = 0;
			pr = &proctab[i];				//Copies the value table
			/*------- Converting to respective no of calls ---------- */
			if ( pr ->prSystemcallCount != 0 ) {
			for ( j = 0; j < (pr-> prSystemcallCount) ; j++ ){
				
					switch (pr -> prSystemcallArray[j]){
							
							case 1: printArray[0]++; break;
							case 2:	printArray[1]++; break;
							case 3: printArray[2]++; break; 
							case 4: printArray[3]++; break;
							case 5:	printArray[4]++; break;
							case 6: printArray[5]++; break;
							case 7: printArray[6]++; break;
							case 8:	printArray[7]++; break;
							case 9: printArray[8]++; break;
							case 10: printArray[9]++; break;						
					}	
			}
			

			kprintf ("%d\tInit\t\t%d\t\t%d\n"	, i, printArray[9], pr -> prInitTime);
			kprintf ("%d\tcreate\t\t%d\t\t%d\n"	, i, printArray[0], pr -> prCreateTime);
			kprintf ("%d\twait\t\t%d\t\t%d\n"		, i, printArray[1], pr -> prWaitTime);
			kprintf ("%d\tresched\t\t%d\t\t%d\n", i, printArray[2], pr -> prReschedTime);
			kprintf ("%d\trecv\t\t%d\t\t%d\n"		, i, printArray[3], pr -> prRecvTime);
			kprintf ("%d\trecvt\t\t%d\t\t%d\n"	, i, printArray[4], pr -> prRecvtTime);
			kprintf ("%d\tready\t\t%d\t\t%d\n"	, i, printArray[5], pr -> prReadyTime);
			kprintf ("%d\tsleep\t\t%d\t\t%d\n"	, i, printArray[6], pr -> prSleepTime);
			kprintf ("%d\tsuspend\t\t%d\t\t%d\n", i, printArray[7], pr -> prSuspendTime);
			kprintf ("%d\tkill\t\t%d\t\t%d\n"		, i, printArray[8], pr -> prKillTime);
			kprintf ("------------------------------------------------------------\n");
			}

	}
	return OK;
}
