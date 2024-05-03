#include "Clock/headers.h"

/* Modify this file as needed*/
int remainingtime;

int prev, curr;

void SIGCONThandler (int signum);

int main(int agrc, char * argv[])
{
    initClk();
    int runningtime = atoi(argv[1]);
    int arrivaltime = atoi(argv[2]);
    signal(SIGCONT, SIGCONThandler);

    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    remainingtime = runningtime;
    // printf("Starting process with ID = %d - Remaining time = %d - Started at time = %d\n", getpid(), remainingtime, getClk());

    prev = getClk();
    while (remainingtime > 0 )
    {      
      curr = getClk();
      if (curr != prev)
      { 
      //  printf("curr = %d - prev = %d\n", curr, prev);
        if (curr >prev)
        {
          remainingtime--;
      //    printf("PID = %d - Remaining time = %d\n", getpid(), remainingtime);
        }
        prev = getClk();   
      }
    }
   // printf("Process with pid = %d - Finish time = %d\n",getpid(), prev);
    kill(getppid(), SIGUSR1);
    destroyClk(false);
    raise(SIGTERM);
    
    return 0;
}

void SIGCONThandler(int signum)
{
 //printf("ID = %d\n", getpid());
 prev = curr = getClk();
}