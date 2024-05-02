#include "Clock/headers.h"



/* Modify this file as needed*/
int turn_shmid; // to know if its their turn
int *turn_shmaddr;
int remainingtime;

void SIGCONThandler (int signum);
void waiting();

int main(int agrc, char * argv[])
{
    initClk();

    signal(SIGCONT, SIGCONThandler);
    int runningtime = atoi(argv[1]);
    int arrivaltime = atoi(argv[2]);

    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    remainingtime = runningtime;
    printf("Starting process with ID = %d - Remaining time = %d\n", getpid(), remainingtime);

    waiting();
   
    printf("Process with pid = %d - Finish time = %d\n",getpid(), getClk());
    kill(getppid(), SIGUSR1);

    destroyClk(false);
    
    return 0;
}

void waiting() {
  int prev = getClk();
  //printf("Start run ID: %d at time: %d and rem time = %d\n", getpid(), getClk(), remainingtime);
  while (remainingtime > 0 )
      {      
        int curr = getClk();
        if (curr != prev)
        {
          prev = getClk();   
          remainingtime --;   
          printf("Process ID = %d - Remaining time = %d\n", getpid(), remainingtime);
        }
      }
}

void SIGCONThandler(int signum)
{
  waiting();
  signal(SIGCONT, SIGCONThandler);
}

