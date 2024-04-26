#include "./Clock/headers.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <./Scheduler/scheduler.c>

void clearResources(int);
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO: Initialization

    FILE *file;
    char line[256];
    int id, arrivalTime, runningTime, priority;

    // 1. Read the input files.
    // Open processes file
    file = fopen("processes.txt", "r");

    // Check if the file exist
    if (file == NULL)
    {
        perror("Error opening file");
        exit(-1);
    }
    // Skip the first line
    fgets(line, sizeof(line), file);

    // Read process data from the file
    while (fscanf(file, "%d %d %d %d", &id, &arrivalTime, &runningTime, &priority) == 4)
    {
        printf("Process ID: %d, Arrival Time: %d, Running Time: %d, Priority: %d\n", id, arrivalTime, runningTime, priority);
    }
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int algorithm;
    int RR;

    // Display menu to the user
    printf("Choose a scheduling algorithm:\n");
    printf("1. Non-preemptive Highest Priority First (HPF)\n");
    printf("2. Shortest Remaining Time Next (SRTN)\n");
    printf("3. Round Robin (RR)\n");
    printf("Enter your choice (1-3): ");

    scanf("%d", &algorithm);

    while (algorithm < 1 || algorithm > 3)
    {
        printf("Invalid choice. Please enter a number between 1 and 3.\n");
        scanf("%d", &algorithm);
    }

    if (algorithm == 3)
    {
        printf("Enter the time slice for Round Robin: ");
        scanf("%d", &RR);
    }

    printf("Chosen algorithm: %d\n", algorithm);
    // 3. Initiate and create the scheduler and clock processes.

    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this

    int x = getClk();
    printf("current time is %d\n", x);
    // TODO: Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
    fclose(file);
    return 0;
}

void clearResources(int signum)
{
    // TODO: Clears all resources in case of interruption
}
