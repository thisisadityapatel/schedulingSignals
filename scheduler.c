#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "queue.h"

// signal handler for SIGUSR1 - sent by worker processes after their completetion
void comp_handler(int sig)
{
    pid_t sender_pid = getpid();
    printf("Completion signal %d from process with PID %d\n", sig, sender_pid);
}

int main(int argc, char *argv[])
{
    // check arguments
    if (argc != 4)
    {
        printf("Usage: %s W(Total worker processes) T(Timeslice) SA(Algorithm: RR[RoundRobin] / L7[Lucky 7] )\n", argv[0]);
        return 1;
    }

    // accessing the arguments
    int W = atoi(argv[1]);
    int timeslice = atoi(argv[2]);
    char *SA = argv[3];

    // anitializing circular queue
    Queue queue;
    Worker workers[W];
    queue.workers = workers;
    queue.allComplete = 0;

    // anitializing signal detector for SIGRTMIN - sent by worker processes once initialized
    sigset_t sigset;
    int sig;
    siginfo_t info;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGRTMIN);

    // block signals in sigset
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    printf("\nWaiting for %d worker processes to initialize...\n\n", W);

    // wait for SIGRTMIN signals from W workers
    for (int i = 0; i < W; i++)
    {
        sig = sigwaitinfo(&sigset, &info);

        if (sig == SIGRTMIN)
        {
            // initializing & adding the worker process to the queue
            int has7 = pidHas7(info.si_pid);
            enqueWorker(&queue, info.si_pid, has7, i);

            printf("Received SIGRTMIN signal from worker with PID %d. PID has 7?: %d\n", info.si_pid, has7);
        }
    }

    printf("\nAll worker processes successfully initialised!!\n");

    // Scheduling the processes and alotting their timeslice
    printf("\nScheduling Worker Proccesses.....\n");

    // handling the SIGUSR1 signals from completed workers
    signal(SIGUSR1, comp_handler);

    int all_complete = 0; // variable to check if all the workers processes are completed
    int i = -1;

    while (queue.allComplete < W)
    {
        nextElement(&i, W);

        if (!queue.workers[i].isComplete)
        {
            int result = kill(queue.workers[i].worker_pid, 0); // checking if the worker processes is running.

            if (result == 0)
            {                                               // if worker still running
                kill(queue.workers[i].worker_pid, SIGCONT); // continue worker process
                sleep(timeslice);                           // non-busy timeslice wait (default)

                // For L7 Scheduling algorithm
                if (queue.workers[i].has7 == 1 && strcmp(SA, "L7") == 0)
                {
                    sleep(timeslice); // additional non-busy timeslice
                }
                kill(queue.workers[i].worker_pid, SIGSTOP); // hault worker process
            }
            else
            {
                queue.workers[i].isComplete = 1; // mark process completed
                queue.allComplete += 1;          // adding one process to completed
            }
        }
    }

    printf("\n-> all worker processes completed.");
    printf("\n-> scheduler completed and terminated.\n");

    return 0;
}