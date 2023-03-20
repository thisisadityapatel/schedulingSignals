#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#define TOTAL_INPUT 40

// function to write into the .txt file
void write_to_file(char *filename)
{
    FILE *fp;
    fp = fopen(filename, "a");
    for (int i = 0; i < TOTAL_INPUT; i++)
    {
        fprintf(fp, "*");
        fflush(fp);
        sleep(1);
    }
    fclose(fp);
}

// function to pause the process
void stop_worker_process()
{
    pause();
    return;
}

// function to resume the process
void resume_worker_process()
{
    return;
}

int main(int argc, char *argv[])
{
    // check arguments
    if (argc != 3)
    {
        printf("Usage: %s filename(worker filename) sPID(scheduler pid)\n", argv[0]);
        return 1;
    }

    int sPID = atoi(argv[2]);
    char *filename = argv[1];

    // worker initialization signal directed to the scheduler using 'SIGRTMIN'
    kill(sPID, SIGRTMIN);

    // receiving the SIGOUT signal to resume the process operations
    signal(SIGCONT, resume_worker_process);

    // receiving the SIGSTOP signal to halt the process operations
    signal(SIGSTOP, stop_worker_process);

    // pausing the current worker process after initialization
    pause();

    // function that deals with writing * into the text file
    write_to_file(filename);

    // notifying scheduler about process completion with SIGUSR1 signal
    kill(sPID, SIGUSR1);

    return 0;
}