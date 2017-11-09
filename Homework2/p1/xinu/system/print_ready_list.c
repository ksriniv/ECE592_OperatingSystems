#include <xinu.h>

syscall print_ready_list (void) {

 int head_k = firstkey(readylist);

/*
 printf (" The key, head and tail are %d %d %d", first_k, head_k, tail_k);
*/


printf (" Processes in the Ready list are :\n");
printf (" Qprev \t Key(Priority) \t Qnext\n");
while (queuetab[head_k].qnext != EMPTY ) {
        printf (" %d \t %d \t\t %d\n", queuetab[head_k].qprev, head_k, queuetab[head_k].qnext ); 
	head_k = queuetab[head_k].qnext;
 }

 
return 1;
}
