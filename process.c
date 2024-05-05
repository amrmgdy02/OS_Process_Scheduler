#include "Clock/headers.h"

/* Modify this file as needed*/
int remainingtime;

int prev, curr, counter = 0;

void SIGCONThandler(int signum);

int main(int agrc, char *argv[])
{
  initClk();
  int runningtime = atoi(argv[1]);
  int quantum     = atoi(argv[2]);

  signal(SIGCONT, SIGCONThandler);

  remainingtime = runningtime;
  // printf("PID = %d initialized.\n", getpid());
  // printf("Starting process with ID = %d - Remaining time = %d - Started at time = %d\n", getpid(), remainingtime, getClk());

  prev = getClk();
  while (remainingtime > 0)
  {
    curr = getClk();
    if (curr != prev)
    {
      if (curr > prev)
      {
      //  printf("curr = %d - prev = %d\n", curr, prev);
        remainingtime--;
        //    printf("PID = %d - Remaining time = %d\n", getpid(), remainingtime);
        counter++; 
        if (counter == quantum && remainingtime > 0)
        {
          kill(getppid(), SIGUSR2);
          counter = 0;
        }
      }
      prev = getClk();
    }
  }
  // printf("Process with pid = %d - Finish time = %d\n",getpid(), prev);
  kill(getppid(), SIGUSR1);
  kill(getppid(), SIGUSR2);
  destroyClk(false);
  raise(SIGTERM);

  return 0;
}

void SIGCONThandler(int signum)
{
  // printf("ID = %d\n", getpid());
  prev = curr = getClk();
}