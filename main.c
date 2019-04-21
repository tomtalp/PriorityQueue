#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 
#include <string.h>

#define MAX_TASKS 20
#define TASK_NAME_LENGTH 2
#define MAX_INPUT_ROW_LEN 10

#define RAND_TIME_UPPER_RANGE 50
#define RAND_TIME_LOWER_RANGE 0

#define RAND_PRIORITY_UPPER_RANGE 10
#define RAND_PRIORITY_LOWER_RANGE 0

#define RAND_NEW_TASK_PROB_UPPER_RANGE 100
#define RAND_NEW_TASK_PROB_LOWER_RANGE 0
#define RAND_NEW_TASK_PROB 2

typedef struct Task {
    int taskPriority;
    int taskDuration;
    char taskName[TASK_NAME_LENGTH];
} Task;

typedef struct TaskQueue {
    Task *tasks[MAX_TASKS];
    int tasksCounter;
} TaskQueue;

void run(TaskQueue *taskQueue);
Task *createNewTask(char *taskName, int taskPriority, int taskDuration);
void printTask(Task *task);
void initTaskQueue(TaskQueue *tq);
void initTaskQueueFromFile(TaskQueue *tq, FILE *fp);
void printTaskQueue(TaskQueue *tq);
int getLeftNode(int i);
int getRightNode(int i);
int getParentNode(int i);
void Heapify(TaskQueue *tq, int i);
void addToTaskQueue(TaskQueue *tq, Task *newTask);
Task *getMaxTask(TaskQueue *tq);
int getRandomNumber(int lowerLimit, int upperLimit);
Task *getRandomTask();


/*
    Create a new task from the given parameters

    @param char *taskName - The name for the new task
    @param int taskPriority  - The priority for the new task
    @param int taskDuration - The duration for the new task

    @return Task* - The newly created task

*/
Task *createNewTask(char *taskName, int taskPriority, int taskDuration) {
    Task *newTask = (Task*)malloc(sizeof(Task));
    strcpy(newTask->taskName, taskName);
    newTask->taskPriority = taskPriority;
    newTask->taskDuration = taskDuration;

    return newTask;
}

/*
    Pretty print for a task

    @param Task *Task - The task to be printed
*/ 
void printTask(Task *task) {
    printf("%s  %d  %d ", task->taskName, task->taskPriority, task->taskDuration);
}

/*
    Naive initialization for the queue, but simply setting the tasks counter to 0

    @param TaskQueue *tq - The task queue we're initializting
*/
void initTaskQueue(TaskQueue *tq) {
    tq->tasksCounter = 0;
}

/*
    Initialize a TaskQueue from a file.
    We read the file line by line and create new tasks for every line in the file.
    Expected line format - "<name> <priority> <duration>" (space delimited)

    @param TaskQueue *tq - The task queue we're initializting
    @param FILE *fp - The file we're reading from.
*/
void initTaskQueueFromFile(TaskQueue *tq, FILE *fp) {
    int priority;
    int duration;
    char name[TASK_NAME_LENGTH];
    char row[MAX_INPUT_ROW_LEN];

    initTaskQueue(tq);

    while (fgets(row, MAX_INPUT_ROW_LEN, fp) != NULL) {
        sscanf(row, "%s %d %d", name, &priority, &duration);    
        addToTaskQueue(tq, createNewTask(name, priority, duration));
    }
}

/*
    Print a task queue so that it kind of looks ike a tree

    @param TaskQueue *tq - The task queue we're printing
*/
void printTaskQueue(TaskQueue *tq) {
    printf("Printing a task queue with %d tasks!\n", tq->tasksCounter);
    int i = 0;
    int j = 0;
    int z = 0;

    while (i < tq->tasksCounter) {
        i = pow(2, z);
        for (j = 0; j < i && (i+j) <= tq->tasksCounter; j++) {
            printf("%d ", tq->tasks[i+j-1]->taskPriority);
        }
        printf("\n");
        z++;
    }
}

/*
    Receive the index of the left node, from the given index

    @param int i - Current index
    @return int - The left node index
*/
int getLeftNode(int i) {
    return 2*i + 1;
}

/*
    Receive the index of the right node, from the given index

    @param int i - Current index
    @return int - The right node index
*/
int getRightNode(int i) {
    return getLeftNode(i) + 1;
}

/*
    Receive the index of the parent node, from the given index

    @param int i - Current index
    @return int - The parent node index
*/
int getParentNode(int i) {
    return (i-1)/2;
}

/*
    Perform the Heapify algorithm on our task queue, in position i.
    Assuming the leaf nodes of i are valid, the heapify method will make sure that the node 
    in position i will go to it's right position

    @param TaskQueue *tq - The task queue we're working with
    @param int i - Position of the node we want to perform Heapify on
*/
void Heapify(TaskQueue *tq, int i) {
    int left = getLeftNode(i);
    int right = getRightNode(i);

    int largest;

    Task *tmp;

    if (left < tq->tasksCounter && tq->tasks[left]->taskPriority > tq->tasks[i]->taskPriority) {
        largest = left;
    } else {
        largest = i;
    }

    if (right < tq->tasksCounter && tq->tasks[right]->taskPriority > tq->tasks[largest]->taskPriority) {
        largest = right;
    }

    if (largest != i) {
        tmp = tq->tasks[largest];
        tq->tasks[largest] = tq->tasks[i];
        tq->tasks[i] = tmp;

        Heapify(tq, largest);
    }
}

/*
    Add a new task to our task queue, in it's correct position - We add it as a leaf,
    and "slide" it all the way up the heap until we get to a place where the parent node's priority
    is larger than the new task's priority

    @param TaskQueue *tq - The task queue we're adding thet ask to
    @param Task *newTask - The new task that's added
*/
void addToTaskQueue(TaskQueue *tq, Task *newTask) {
    Task *tmp;
    int newPos = tq->tasksCounter;

    tq->tasksCounter += 1;
    tq->tasks[newPos] = newTask; // Put the new task at the end of the heap

    // As long as the new priority is bigger than the parent node priority, we keep going up
    while (newPos > 0 && tq->tasks[newPos]->taskPriority > tq->tasks[getParentNode(newPos)]->taskPriority) {
        tmp = tq->tasks[getParentNode(newPos)];
        tq->tasks[getParentNode(newPos)] = tq->tasks[newPos];
        tq->tasks[newPos] = tmp;

        newPos = getParentNode(newPos);
    }

}

/*
    Receive a TaskQueue that implements MaxHeap and return the highest ranked "task", while still 
    maintaining the max heap structure.

    This is done by taking the first node in the heap, and then running the heapify method.

    @param TaskQueue *tq - The task queue we're working with
    @return Task* - Pointer to the max task we retrieved from the queue
*/
Task *getMaxTask(TaskQueue *tq) {
    Task *tmp;
    Task *max;

    max = tq->tasks[0];

    tmp = tq->tasks[tq->tasksCounter-1];
    tq->tasks[0] = tmp;
    tq->tasksCounter--;

    Heapify(tq, 0);
    return max;
}

/*
    Randomly generate a number, given an upper & lower bound

    @param int lowerLimit - Lower bound for our random number
    @param int upperLimit - upper bound for our random number

    @return int - The randomly selected number
*/
int getRandomNumber(int lowerLimit, int upperLimit) {
    return (rand() % (upperLimit - lowerLimit + 1)) + lowerLimit; 
}

/*
    Randomly generate a task

    @return Task* - Pointer to the new task
*/
Task *getRandomTask() {
    int priority = getRandomNumber(RAND_PRIORITY_LOWER_RANGE, RAND_PRIORITY_UPPER_RANGE);
    int duration = getRandomNumber(RAND_TIME_LOWER_RANGE, RAND_TIME_UPPER_RANGE);
    char name[TASK_NAME_LENGTH+1];

    sprintf(name, "W%d", getRandomNumber(1, 10));
    createNewTask(name, priority, duration);
}

/*
    Receive a task queue and read until it's empty, by taking the task with the highest priority.
    Every time unit that passes, there's a 2% chance that a new task will pop in, which will 
    also be handled (that is, the new random task will be inserted in it's position and handled
    when it's the task with the highest priority)

    @param TaskQueue *taskQueue - A pointer to the task queue we're working with
*/
void run(TaskQueue *taskQueue) {
    printf("Started running!\n");
    Task *currTask, *randTask;
    int totalTime = 0;
    int i = 0;
    
    while (taskQueue->tasksCounter > 0) {
        i++;

        currTask = getMaxTask(taskQueue);

        printf("%d ", i);
        printTask(currTask);
        printf("%d \n", totalTime);

        for (; currTask->taskDuration > 0; currTask->taskDuration--) {
            /* Check if a new task is coming, with a 0.02 probability */
            if (getRandomNumber(RAND_NEW_TASK_PROB_LOWER_RANGE, RAND_NEW_TASK_PROB_UPPER_RANGE) == RAND_NEW_TASK_PROB) {
                randTask = getRandomTask();
                printf("Detected random task! (%s with priority %d duration %d) \n", randTask->taskName, randTask->taskPriority, randTask->taskDuration);
                addToTaskQueue(taskQueue, randTask);
            }
            totalTime++;
        }
    }

    printf("Task pool is now empty! Process took %d time units\n", totalTime);
}

int main() {
    FILE *fp;
    TaskQueue taskQueue;

    srand(time(0)); /* Set the seed for our random variables*/
    
    fp = fopen("maman_14_input.txt", "r");

    if (fp == NULL) {
        printf("FAILED OPENING FILE!\n");
        return 1;
    }

    initTaskQueueFromFile(&taskQueue, fp);

    run(&taskQueue);

    return 0;
}