#include "Clock/headers.h"



/* Modify this file as needed*/
int turn_shmid; // to know if its their turn
int *turn_shmaddr;
int remainingtime;

int prev, curr;
int starttime;
int waitingtime;
char state[10];

void SIGCONThandler (int signum);
void sigtstphandler(int signum);
void waiting();

int main(int agrc, char * argv[])
{
    initClk();
    int runningtime = atoi(argv[1]);
    int arrivaltime = atoi(argv[2]);
    state = "started";

    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    remainingtime = runningtime;


    prev = getClk();
    signal(SIGCONT, SIGCONThandler);
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
          state = "finished";
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

void sigtstphandler(int signum)
{
  raise(SIGSTOP);
}