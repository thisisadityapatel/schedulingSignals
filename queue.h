#ifndef QUEUE_H
#define QUEUE_H

// Worker structure
typedef struct
{
    int worker_pid;
    int isComplete;
    int has7;
} Worker;

// Queue Structure
typedef struct
{
    Worker *workers;
    int size;
    int allComplete;
} Queue;

// Methods
int pidHas7(int worker_pid);
void nextElement(int *pos, int W);
void enqueWorker(Queue *q, int worker_pid, int has7, int position);
#endif