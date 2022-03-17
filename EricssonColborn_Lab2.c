#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Ericsson Colborn
 * OS 202: Operating Systems
 * Professor Gottieb
 * December 2, 2021
 
-----------LAB  2: Scheduling -----------
 
Simulate the scheduling of n processes P1, P2, etc. on
a single CPU. Use both FIFO and RR scheduling.

* Each procees runs twice and blocks twice. Then, to terminate, each
* process runs 1 additional ms.
* The order is R1_i, B1_i, R2_i, B2_i, where i is the process #

*/

/*  STRUCT: process
  
    Simulates the behavior and properties of processes. Contains member variables that are modified
    during the implementation of both scheduling algorithms.

    All member var's are ints:
    - id: process id (pid)
    - state: current process state (0 = ready, 1 = running, 2 = blocked, 3 = preempted, 4 = terminated)
    - r1, r2, b1, b2: corresponding run/block times desginated by user
*/

struct process {

    int id;
    int state;

    int r1;
    int r2;
    int b1;
    int b2;
};


// default "null" processs. All attributes are set to 0.
static struct process nullProcess;


/*  FUNCTION: popuate()

    @params:
    - (struct process*) p: process to be populated
    - (int) id, state, r1, b1, r2, b2: values to populate the process with
*/

int populate (struct process *p, int id, int r1, int b1, int r2, int b2) {

    p->id = id;
    p->r1 = r1;
    p->r2 = r2;
    p->b1 = b1;
    p->b2 = b2;

    p->state = 0;

    return 1;
}


/* ---------- Printing helper Functions ------------

    These functions make it easy to print stuff, essentially.
    Used to format the output.
*/

/*  FUNCTION: print()

    Prints the id and run/block times of a proces.

    @param: (struct process*) p: pointer to the process that is to be printed
    @return: void
*/

void print(struct process *p) {

    if (p->id == 0) {
        
        printf("Null process\n");
        return;
    }
    printf("\nProcess ID: %d\n", p->id);
    printf("R1: %d  B1: %d  R2: %d  B2: %d State: %d\n",
            p->r1, p->b1, p->r2, p->b2, p->state);
}


/*  FUNCTIONS: ready(), blocked(), running(), terminated():

    - print the corresponding word, formatted correctly for output
*/

void ready() {

    char c[10] = "Ready";
    printf("%-*s", 12, c);
}


void blocked() {

    char c[10] = "Blocked";
    printf("%-*s", 12, c);
}


void running() {

    char c[10] = "Running";
    printf("%-*s", 12, c);
}


void terminated() {

    char c[12] = "Terminated";
    printf("%-*s", 12, c);
}


/*  FUNCTION: printCycle()

    Prints a line of output, which consists of:
    Cycle#    P1 State    P2 State    P3 State...

    @params:
    - (int) cycle: current cycle
    - (struct process) processArray[]: array of processes
    - (int) n: # of processes 

    @return: void
*/

void printCycle (int cycle, struct process processArray[], int n) {

    printf("\n%-*d", 8, cycle);

    int i;
    for (i = 0; i < n; i++) {

        if (processArray[i].state == 0) {
            
            ready(); 
            continue;

        } else if (processArray[i].state == 1) {

            running();
            continue;

        } else if (processArray[i].state == 2) {

            blocked();
            continue;
        
        } else if (processArray[i].state == 4) {

            terminated();
            continue;

        } else printf("Error reading process state");

        printf("\n");
    }
}


void printRL(int readyList[], int n) {

    int i;
    printf("    Ready List: ");
    for (i = 0; i < n; i++) {

        printf("%d ", readyList[i]);
    }
}

void printBL(int blockedList[][2], int n) {

    int i;
    printf("    Blocked List: ");
    for (i = 1; i <= n; i++) {

        printf("(%d, %d) ", blockedList[i][0], blockedList[i][1]);
    }
}


/* ------------- QUEUE METHODS  ---------------

    The enqueue() and dequeue() functions implement their respective queue methods traditionally seen
    in other programming langages.
    The acutaly queue is implemented with an array in main().
*/

/*  FUNCTION: dequeue()

    Dequeue the list of ready processes (Ready List).

    @params:
    - (int) readyList[]: integer array containing all ready processes in the order
            they entered the ready state.
    - (int) n: # of processes

    @return: void
*/

void dequeue (int readyList[], int n) {

    int i;
    
    for (i = 1; i < n; i++) {

        readyList[i-1] = readyList[i];
    }

    readyList[n-1] = 0;

    return;
}


/*  FUNCTION: enqueue()

    Enqueue the list of ready processes (Ready List).

    @params:
    - (int) readyList[]: integer array containing all ready processes in the order
            they entered the ready state.
    - (int) n: # of processes

    @return: void
*/

void enqueue (int pid, int readyList[], int n) {

    readyList[n-1] = pid;
    //printf("im not crazy, input is %d, readyList[n-1] = %d\n", pid, readyList[n-1]);
    int i = n-1;
    while (readyList[i-1]  == 0 && i > 0) {

        //printf("shifting...");
        readyList[i-1] = pid;
        readyList[i] = 0;
        i--;
    }
    return;
}


/* ------------ STATE CHANGE HELPER FUNCTIONS --------------

    These functions modularize the actions of the "OS" in every situation.
    Block, unblock, preempt, and run are implemented separately.
    There is no terminate function.
*/

/*  FUNCTION: block()

    Blocks the given process. Changes state to 2 (blocked).
    Updates the status of the process in the Blocked List.

    @param: process to be blocked
    @return: cycle # that the process unblocks at
*/

int block (struct process *p, int *num_blocked, int blockedList[][2], int startCycle, int n) {

    p->state = 2;
    // switch flag to 1 ("blocked")
    blockedList[p->id][0] = 1;

    // has not been blocked before
    if (p->b1 > 0) blockedList[p->id][1] = startCycle + p->b1;

    // has already been blocked once
    else if (p->b2 > 0) blockedList[p->id][1] = startCycle + p->b2;

    (*num_blocked)++;
    return blockedList[p->id][1];
}


/*
    FUNCTION - unblocks()

    Unblocks the given process. Change state from 2 (blocked) to 0 (ready).
    Updates the status of the process in the Blocked List.
    
    @param: process to be unblocked
    @return:
    - if at least one proces is ready -> 0
    - if all processes are blocked or terminated -> process id (PID) so this one can be run immediately
*/

// "empty" slot in blockedList => pid = 0, unblock cycle = 0
int unblock (int i, struct process *p, int *num_blocked, int blockedList[][2], int n, int num_terminations) {

    int pid = 0;
    p->state = 0;
    // reset flag to "unblocked"
    blockedList[i][0] = 0;
    // reset time left to block
    blockedList[i][1] = 0;

    // if its the first block...
    if (p->b1 > 0) {

        p->b1 = 0;
    
    // else if its second block
    } else if (p->b2 > 0) {

        p->b2 = 0;

    } else {

        printf("Error in unblock()");
        exit(1);
    
    } if ((*num_blocked) + num_terminations == n) {

        p->state = 1;
        pid = p->id;
    }

    (*num_blocked)--;
    return pid;
}


/*
    FUNCTION - preempt()

    Preempts the given process. Change state from 1 (running) to 0 (ready).
    
    @param: process to be preempted
    @return:
    - 0 if no process is ready
    - 1 if there is at least one ready process
*/

int preempt(struct process* p, int readyList[]) {

    p->state = 0;
    if (readyList[0] == 0) {

        return 0; // should trigger running = nullPrcoess in RR functions
   
    } else {

        return 1; // should trigger running = readyList[0]
    }
}


/*
    FUNCTION - run()

    Runs the given process. Changes state to 1 (running).

    @param: process to be run
    @return:
    - 0 if the process blocks
    - >0 if the process still needs more time to run (returns time left to run)
    - -1 if the process terminates 
    - -2 if an error occured
*/

int run(struct process* p) {

    if (p->id == -5) return -5;
    if (p->state == 4) return -1;
    p->state = 1;

    if (p->r2 < 0) {

        return -2;
    }

    // if p->r1 == 0, then it's on its second run.
    // No need to check that, however -> it'll automatically pass over the R1 conditions

    // more to go on run 1
    else if (p->r1 > 1) {

        (p->r1)--;
        return p->r1; // >0
    
    // last cycle of run 1, time to block
    } else if (p->r1 == 1) {

        (p->r1)--;
        return p->r1; // 0
    
    // more to go on run 2
    } else if (p->r2 > 1) {

        (p->r2)--;
        return p->r2; // >0
    }

    // last cycle of run 2, time to block again
    else if (p->r2 == 1) {

        (p->r2)--;
        return p->r2; // 0
    }

    // finished both r1 and r2, so run once more to terminate
    else if (p->r2 == 0) 
        
        (p->r2)--;
        //(p->state) = 4; // terminated state
        return p->r2; // -1

    return -2;
}


/* -------------- SCHEDULING ALGORITHMN FUNCTIONS ------------------

    The fifo() and rr() functions implement their respective scheudling algorithmns.
    Called in main().
*/

/* FUNCTION - rr()

   Actual implementation of the Round Robin (RR) algorithmn used by the program.
   Called on SA = 2 in main().

   @param: 
    - (struct process) processArray[]: array of processes, state and run/block values
        are modified by the function.
    - (int) n: # of processes (aka the size of the processArray).
    - (int) q: quantum to be employed.

   @return: void
*/

void rr (struct process processArray[], int n, int q) {

    struct process nullProcess;
    nullProcess.id = -5;

    int readyList[n];
    int blockedList[n+1][2]; // col 0: pid, col 1: when to unblock (cycle #)
    int i;

    // initialize ready list with all processes id's
    for (i = 0; i < n; i++) {

        readyList[i] = processArray[i].id;
        blockedList[i+1][0] = 0;
        blockedList[i+1][1] = 0;
    }

    int num_terminations = 0;
    int num_blocked = 0;
    int cycle = 1;
    struct process* running = &processArray[readyList[0]-1];
    struct process *temp;
    int keep_running = run(running);
    dequeue(readyList, n);

    // main loop
    // while at least one process survives...
    int count = 0;
    int current_q = 0;
    while (num_terminations < n) {

        current_q++;
        printCycle(cycle, processArray, n);

        // if the current process needs more time to run...

        // if the process blocks...
        if (keep_running == 0) {
            
            current_q = 0;
            
            int unblock_at = block(running, &num_blocked, blockedList, cycle, n);
        
            if (readyList[0] != 0) {

                running = &processArray[readyList[0]-1];
                dequeue(readyList, n);

            } else running = &nullProcess;

        // unblock processes that are done with "I/O"
        } for (i = 1; i <= n; i++) {

            // if the process is blocked and set to unblock this cycle...
            if (blockedList[i][1] == cycle) {

                enqueue(i, readyList, n);
                int pid = unblock(i, &processArray[i-1], &num_blocked, blockedList, n, num_terminations);
                
                if (pid > 0) {

                    running = &processArray[pid-1];
                    dequeue(readyList, n);
                }
            }
        }

        // if the process terminates..
        if (keep_running == -1) {

            current_q = 0;
            running->state = 4;
            
            if (readyList[0] != 0) {

                running = &processArray[readyList[0]-1];
                dequeue(readyList, n);
            
            } else {

                running = &nullProcess;
            }
            
            num_terminations++;
        }


        // if process doesn't block/terminate but uses up its alloted q...
        if (current_q == q) {

            temp = running;
            if (preempt(running, readyList) == 0) {

                // go for another quantum, back to running state
                running->state = 1;

            } else {

                // get preempted
                running =  &processArray[readyList[0]-1];
                dequeue(readyList, n); 
                enqueue(temp->id, readyList, n);
            }

            current_q = 0;
            temp = NULL;
            
        // if it still has q left...
        } if (keep_running == 1) {}
        
        // or no processes can be run...
        else if (keep_running == -5) {}

        // run process
        keep_running = run(running);
        // keep track of the cycle (in ms)
        cycle++;
        count++;
    }

    // print the last line of results once all processes terminate
    printCycle(cycle, processArray, n);

} // end RR


/* FUNCTION - fifo()

   Actual implementation of the First-In, First-Out (FIFO) algorithmn used by the program.
   Called on SA = 1 in main().

   @param: 
    - (struct process) processArray[]: array of processes, state and run/block values
        are modified by the function.
    - (int) n: # of processes (aka the size of the processArray).

   @return: void
*/

void fifo (struct process processArray[], int n) {

    struct process nullProcess;
    nullProcess.id = -5;

    int readyList[n];
    int blockedList[n+1][2]; // col 0: pid, col 1: when to unblock (cycle #)
    int i;

    // initialize ready list with all processes id's
    for (i = 0; i < n; i++) {

        readyList[i] =  i+1; //processArray[i].id;
        blockedList[i+1][0] = 0;
        blockedList[i+1][1] = 0;
    }

    int num_terminations = 0;
    int num_blocked = 0;
    int cycle = 1;
    struct process* running = &processArray[readyList[0]-1];
    int keep_running = run(running);
    dequeue(readyList, n);
    
    // main loop
    // while at least one process survives...
    int count = 0;
    while (num_terminations < n) {

        printCycle(cycle, processArray, n);

        // if the current process needs more time to run
        if (keep_running == 1) {}
    
        // otherwise if the process blocks
        else if (keep_running == 0) {
            
            block(running, &num_blocked, blockedList, cycle, n);
            if (readyList[0] != 0) {

                running = &processArray[readyList[0]-1];
                dequeue(readyList, n);

            } else running = &nullProcess;

        
        // if the process terminates
        } else if (keep_running == -1) {

            running->state = 4;
            
            if (readyList[0] != 0) {

                running = &processArray[readyList[0]-1];
                dequeue(readyList, n);
            
            } else {

                running = &nullProcess;
            }
            
            num_terminations++;
        }

        // no process can be ran
        else if (keep_running == -5) {}

        // unblock processes that are done with "I/O"
        int i;
        for (i = 1; i <= n; i++) {

            if (blockedList[i][1] == cycle) {

                //printf("    unblocking p%d...", i);

                enqueue(i, readyList, n);
                //printRL(readyList, n, num_blocked, num_terminations);
                int pid = unblock(i, &processArray[i-1], &num_blocked, blockedList, n, num_terminations);
                if (pid > 0) {

                    running = &processArray[pid-1];
                    dequeue(readyList, n);
                }
            }
        }

        // run process
        keep_running = run(running);
        // keep track of the cycle (in ms)
        cycle++;
        count++;
    }

    // print the last line of results once all processes terminate
    printCycle(cycle, processArray, n);

} // end FIFO


// ---------------- Main Program -------------------

int main () {

    // intialize data structures to store each input
    int n, SA, q = -1;
    int pr_table[100][4];  // store 100 processes, each with R1, B1, R2, B2 values

    // obtain and store values for n, SA, and q (if applicable)
    // validate input at each step (n > 0, SA in {1, 2}, q > 0)
    scanf("%d", &n);
    while (n <= 0) {

        printf("n must be > 0.");
        scanf("%d", &n);
    }

    scanf("%d", &SA);
    while (!(SA == 1 || SA == 2)) {

        printf("SA must be either 1 (FIFO) or 2 (RR).");
         scanf("%d", &SA);
    }

    if (SA == 2) {
        
        scanf("%d", &q);
        while (q <= 0) {

            printf("q must be > 0.");
            scanf("%d", &q);
        }
    }

    // print values back to user
    printf("n = %d, SA = %d", n, SA);
    if (SA == 1) printf("\n\n");
    else if (SA == 2) printf(", q = %d\n\n", q);

    // obtain and store R1, B1, R2, B2 values for each process
    int pr_num = 0, value_index = 0;
    while (pr_num < n) {

        int i;
        int temp = 0;

        for (i = 0; i < 4; i++) {

            scanf("%d", &temp);
            
            while (temp <= 0) {

                printf("Run and block values must be > 0.\n");
                scanf("%d", &temp);   
            }
            
            pr_table[pr_num][value_index] = temp;
            value_index++;
        }

        pr_num++;
        value_index = 0;
    }

    // array to store all processes (size given by user)
    struct process processArray[n];

    // initialize structs, populate the array
    int i;
    for (i = 0; i < n; i++) {
    
        struct process p;
        populate(&p, i+1, pr_table[i][0], pr_table[i][1],
                  pr_table[i][2], pr_table[i][3]);
        memcpy(&processArray[i], &p, sizeof(p));
    }
    
    // ------------- PRINTING OUTPUT ------------------

    // Cycle header
    printf("Cycle   "); 

    // for each process, print a State header
    for (i = 0; i < pr_num; i++) {

        printf("P%d State    ", i+1); // 12 spaces long
    }
    printf("\n");

    // run corresponding function for the given scheduling algorithmn     
    if (SA == 1) fifo(processArray, n);
    else if ( SA == 2) rr(processArray, n, q);


    else {

        printf("Invalid scheduling algorithmn.\n");
        exit(1);
    }
} // end main