#include "Clock/headers.h"

/* Modify this file as needed*/
int remainingtime;

int prev, curr;

void SIGCONThandler(int signum);
void RRSIGCONThandler(int signum);

int main(int agrc, char *argv[])
{
  initClk();
  int runningtime = atoi(argv[1]);
  int algorithm   = atoi(argv[2]);

  // printf("%d\n", algorithm);
  if (algorithm == 3)
    signal(SIGCONT, RRSIGCONThandler);
  else
    signal(SIGCONT, SIGCONThandler);

  // TODO it needs to get the remaining time from somewhere
  // remainingtime = ??;
  remainingtime = runningtime;
  printf("PID = %d initialized.\n", getpid());
  // printf("Starting process with ID = %d - Remaining time = %d - Started at time = %d\n", getpid(), remainingtime, getClk());

  if (algorithm != 3)
  {
    prev = getClk();
    while (remainingtime > 0)
    {
      curr = getClk();
      if (curr != prev)
      {
        //  printf("curr = %d - prev = %d\n", curr, prev);
        if (curr > prev)
        {
          remainingtime--;
          //    printf("PID = %d - Remaining time = %d\n", getpid(), remainingtime);
        }
        prev = getClk();
      }
    }
    // printf("Process with pid = %d - Finish time = %d\n",getpid(), prev);
  }
  else
  {
    kill(getpid(), SIGTSTP);
    while (remainingtime > 0);
  }

  kill(getppid(), SIGUSR1);
  destroyClk(false);
  raise(SIGTERM);

  return 0;
}

void SIGCONThandler(int signum)
{
  // printf("ID = %d\n", getpid());
  prev = curr = getClk();
}

void RRSIGCONThandler(int signum)
{
  remainingtime--;
  if (remainingtime == 0)
  {
    // printf("Process with pid = %d finished \n",getpid());
    kill(getppid(), SIGUSR1);
    kill(getppid(), SIGUSR2);
    destroyClk(false);
    raise(SIGTERM);
  }
  kill(getppid(), SIGUSR2);
  // printf("Cont sig: Process with pid = %d - Remaining time = %d\n", getpid(), remainingtime);
}