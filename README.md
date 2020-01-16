# sync_test Throwaway code. Looking at the time difference between
several common synchronization mechanisms for a million iterations of
a producer/consumer loop.

The three mechanisms tried are:

- A pipe. Producer writes a byte after each element is produced. Consumer reads a byte before consuming element.

- A pthread_cond_t condition variable.

- A sem_t couting semaphore. 

The output on my machine (manjaro linux) is: 

     evaitl@bb sync_test]$ ls
     LICENSE  Makefile  README.md  sync.c
     [evaitl@bb sync_test]$ make
     gcc -Wall -O2 -c -o sync.o sync.c
     gcc -Wall -O2 -o sync sync.o -lpthread
     [evaitl@bb sync_test]$ ./sync 
     Pipe. Q = 0, max_q_size 5956, underflows 0
     Time: 0 S:695100442 nS
     Cond var. Q = 0, max_q_size 104162, underflows 0
     Time: 0 S:465343564 nS
     Counting sem. Q = 0, max_q_size 19323, underflows 0
     Time: 0 S:430964583 nS
     [evaitl@bb sync_test]$ ./sync 
     Pipe. Q = 0, max_q_size 5299, underflows 0
     Time: 0 S:739420086 nS
     Cond var. Q = 0, max_q_size 89481, underflows 0
     Time: 0 S:423448141 nS
     Counting sem. Q = 0, max_q_size 2531, underflows 0
     Time: 0 S:418388530 nS
     [evaitl@bb sync_test]$ ./sync 
     Pipe. Q = 0, max_q_size 7543, underflows 0
     Time: 0 S:694078542 nS
     Cond var. Q = 0, max_q_size 334265, underflows 0
     Time: 0 S:398508465 nS
     Counting sem. Q = 0, max_q_size 56804, underflows 0
     Time: 0 S:381196771 nS
