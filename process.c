#include "Clock/headers.h"

/* Modify this file as needed*/
int remainingtime;

void SIGCONThandler (int signum);


int main(int agrc, char * argv[])
{
    initClk();
    signal(SIGCONT, SIGCONThandler);
    int runningtime = atoi(argv[1]);
    int arrivaltime = atoi(argv[2]);
    printf("Process with pid = %d - Run time = %d - Arrival = %d\n Start = %d\n",getpid(), runningtime, arrivaltime, getClk());

    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    remainingtime = runningtime;
    int prev = getClk();
     while (remainingtime > 0)
     {      
        int curr = getClk();
        if (curr != prev)
        {
          prev = getClk();
          remainingtime --;  
        }
     }
   
    printf("\nProcess with pid = %d - Finish time = %d\n",getpid(), getClk());
    kill(getppid(), SIGUSR1);

    destroyClk(false);
    
    return 0;
}

void SIGCONThandler(int signum)
{
  int prev = getClk();
  while (remainingtime > 0)
      {      
        int curr = getClk();
        if (curr != prev)
        {
          prev = getClk();
          remainingtime --;  
        }
      }
  
  signal(SIGCONT, SIGCONThandler);
}