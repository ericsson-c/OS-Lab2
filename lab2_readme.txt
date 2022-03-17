Ericsson Colborn, evc252, N11274170

Operating Systems (OS202), NYU
Professor Gottlieb

---------- Lab 2 README -----------


This lab implements FIFO and RR scheduling algorithms using C. The state of each (simulated) process at every ms is printed to the console via a table (until all processes terminate). No actual processes are created, this lab just simulates their usage. 

How to run / compile
---------------------------------

X86-64 compiler with no additional keywords. Has been tested on local Mac OS terminal and NYU linux server.

Input
---------------------------------
Sequence of integers in the order:

- int n: number of processes
- int SA: Scheduling algorithm. 0 = FIFO, 1 = RR

Then, for each process,

- int R1: # of cycles for its first run
- int B1: # of cycles for its first block ("I/O")
- int R2: # of cycles for its second run
- int B1: # of cycles for its second block ("I/O")

User must enter all four values are entered in sequence before they enter any values for the next process.

Output
---------------------------------
Table listing the state of each process at each cycle (assume that 1 cycle = 1 ms)

Example run:
---------------------------------

Input: 2 1 2 2 2 2 2 2 2 2

All run and block times = 2

N = 2, SA = 1 (FIFO)

Cycle   P1 State   P2 State

1.      Running    Ready
2.      Running    Ready
3.      Blocked    Running
4.      Blocked    Running
5.      Running    Blocked
6.      Running    Blocked
7.      Blocked    Running
8.      Blocked    Running
9.      Running    Blocked
10.     Terminated Blocked
11.     Terminated Running
12.     Terminated Terminated

* all process must run for 1 cycle after finishing all runs/blocks to "terminate"
