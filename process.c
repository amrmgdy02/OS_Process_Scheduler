#include "Clock/headers.h"

/* Modify this file as needed*/
int remainingtime;

void handler (int signum);


int main(int agrc, char * argv[])
{
    initClk();
   // signal(SIGSTOP, handler);
    int runningtime = atoi(argv[1]);
    int arrivaltime = atoi(argv[2]);
    
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    // while (remainingtime > 0)
    // {
    //     // remainingtime = ??;
    // }
   
    printf("I am process with pid = %d -- My running time is: %d -- my arrival time is: %d\n current time = %d\n",getpid(), runningtime, arrivaltime, getClk());
    
    destroyClk(false);
    
    return 0;
}

// void handler (int signum)
// {
//   printf("I RECIEVED A STOP SIGNAL\n");
//   raise(SIGSTOP);
// }