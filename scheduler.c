#include "DataStructures/PriorityQueue.h"
#include <sys/file.h>
#include <math.h>
#include <stdio.h>


PriorityQueue *PQ = NULL;
Queue *Q = NULL;
Queue *finishedQueue = NULL;

int TA;
float WTA;
float *allWTA;

// set to zero when it receives a termination signal from a process
int flag = 1;
int quantum;

int processCount; // to check if all processes finished or other processes were not sent yet
int algorithm;
int currentProcessCount;
key_t sch_key_id;
int sch_rec_val, sch_msgq_id;
struct msgbuff SCH_message;

int children_shmid;
int *children_shmaddr;

int totalWaitingTime = 0;
float totalWTA = 0;
int totalRT = 0;

FILE *schedulerLog, *schedulerPref;

process *runningProcess = NULL;

/////// FUNCTIONS ////////

int forkNewProcess(char *runtime, char *arrivaltime, int run);
void getAlgorithm();
void connectWithGenerator();
void finishedPhandler(int signum);
void sigtermhandler(int signum);
void RRScheduler(int quantum);
process *initProcess();
void outputFile();

void STRN();
void STRNaddprocess();

void HPF();
void HPFaddprocess();
void printProcessState();
void printProcessFinish();

int main(int argc, char *argv[])
{
  initClk();

  signal(SIGTERM, sigtermhandler);   // to free the allocated memory
  signal(SIGUSR1, finishedPhandler); // to recieve that a process has finished its execution
  //schedulerLog=fopen("scheduler.log", "a+");
  schedulerLog=fopen("scheduler.log", "w");
  //schedulerPref=fopen("scheduler.pref", "a+");
  schedulerPref=fopen("scheduler.pref", "w");
  algorithm = atoi(argv[1]);
  processCount = atoi(argv[2]);
  currentProcessCount = processCount;
  quantum = atoi(argv[3]);
  allWTA = malloc(currentProcessCount * sizeof(float));
  finishedQueue = createQueue();
  connectWithGenerator();

  getAlgorithm();

  // TODO implement the scheduler :)
  outputFile();
  destroyClk(true);
  fclose(schedulerLog);
  fclose(schedulerPref);
  kill(getppid(), SIGINT);
  return 0;
}

void STRN()
{
  PQ = createPriorityQueue();
  while (processCount > 0)
  {
    sch_rec_val = msgrcv(sch_msgq_id, &SCH_message, sizeof(SCH_message.arrivedProcess), getpid(), !IPC_NOWAIT);
    if (sch_rec_val != -1)
    {
      STRNaddprocess();
    }
    if (runningProcess == NULL && !PQisEmpty(PQ))
    {
      runningProcess = PQpeek(PQ);
      kill(runningProcess->realPid, SIGCONT);
      if (runningProcess->remainingtime == runningProcess->runningtime)
      {
        runningProcess->starttime = getClk();
        runningProcess->waitingtime = runningProcess->starttime - (runningProcess->arrivaltime);
        fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d\n",getClk(), runningProcess->id, "started", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
        }
      else
      {
        int tempTime = getClk()-runningProcess->laststoptime;
        runningProcess->waitingtime+=tempTime;
      fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d\n",getClk(), runningProcess->id, "resumed", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      }
      runningProcess->lastRunningClk = getClk();
    }
  }
  free(PQ);
}

void STRNaddprocess()
{
  process *newprocess = initProcess();
  if (runningProcess != NULL)
  {
    runningProcess->remainingtime = runningProcess->remainingtime - (getClk() - runningProcess->lastRunningClk);
    runningProcess->lastRunningClk = getClk();
    STRNenqueue(PQ, newprocess, newprocess->remainingtime);
    if (newprocess->remainingtime < runningProcess->remainingtime)
    {
      kill(runningProcess->realPid, SIGTSTP);
      fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d\n",getClk(), runningProcess->id, "stopped", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      runningProcess->laststoptime = getClk();
      runningProcess = newprocess;
      kill(runningProcess->realPid, SIGCONT);
      runningProcess->lastRunningClk = getClk();
      if (runningProcess->remainingtime == runningProcess->runningtime)
      {
        runningProcess->starttime = getClk();
        runningProcess->waitingtime = runningProcess->starttime - (runningProcess->arrivaltime);
      fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d\n",getClk(), runningProcess->id, "started", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      }
      else
      {
        int tempTime = getClk()-runningProcess->laststoptime;
        runningProcess->waitingtime+=tempTime;
      fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d\n",getClk(), runningProcess->id, "resumed", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      }
    }
  }
  else
  {
    STRNenqueue(PQ, newprocess, newprocess->remainingtime);
  }
}

void HPF()
{
  PQ = createPriorityQueue();
  while (processCount > 0)
  {
    sch_rec_val = msgrcv(sch_msgq_id, &SCH_message, sizeof(SCH_message.arrivedProcess), getpid(), !IPC_NOWAIT);
    if (sch_rec_val != -1)
    {
      HPFaddprocess();
    }
    if (!PQisEmpty(PQ) && runningProcess == NULL)
    {
      runningProcess = PQpeek(PQ);
        runningProcess->starttime = getClk();
        runningProcess->waitingtime = runningProcess->starttime - (runningProcess->arrivaltime);
      fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d\n",getClk(), runningProcess->id, "started", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      runningProcess->lastRunningClk = getClk();
      kill(runningProcess->realPid, SIGCONT);
    }
  }
  free(PQ);
}

void HPFaddprocess()
{
  process *newprocess = initProcess();
  HPFenqueue(PQ, newprocess, newprocess->priority);
}

int forkNewProcess(char *runnungtime, char *arrivaltime, int run)
{
  int id = fork();
  if (id == -1)
  {
    perror("error in fork");
    exit(-1);
  }
  else if (id == 0)
  {
    if (execl("./process.out", "process.out", runnungtime, arrivaltime, NULL) == -1)
    {
      perror("execl: ");
      exit(1);
    }
  }

  kill(id, SIGTSTP); // stop the forked (except if the ready queue is empty) process untill its turn

  return id;
}

///////////////////////////////////////////

void getAlgorithm()
{
  switch (algorithm)
  {
  case 1:
    printf("You are in HPF mode\n");
    HPF();
    break;
  case 2:
    printf("You are in SRTN mode\n");
    STRN();
    break;
  case 3:
    printf("You are in RR mode\n");
    RRScheduler(quantum);
    break;
  }
}

///////////////////////////////////////////

void connectWithGenerator()
{
  sch_key_id = ftok("keyfile", 65);
  sch_msgq_id = msgget(sch_key_id, 0666 | IPC_CREAT);
  if (sch_msgq_id == -1)
  {
    perror("Error in create");
    exit(-1);
  }
}

////////////////////////////////////////////

void sigtermhandler(int signum)
{
  free(Q);
  free(PQ);
  kill(getpgrp(), SIGKILL);
  signal(SIGTERM, sigtermhandler);
}

void finishedPhandler(int signum)
{
  if (algorithm == 2)
  {
    process *finishedprocess = NULL;
    runningProcess = NULL;
    finishedprocess = PQdequeue(PQ);
    printf("Process ID = %d Fininshed at time = %d\n", finishedprocess->id, getClk());
  int TA = getClk() - finishedprocess->arrivaltime;
  float WTA = (float)TA / (float)finishedprocess->runningtime;
  totalWTA += WTA;
  allWTA[processCount-1]=WTA;
  totalWaitingTime += finishedprocess->waitingtime;
totalRT += finishedprocess->runningtime;
  fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), finishedprocess->id, "finished", finishedprocess->arrivaltime, finishedprocess->runningtime, 0, finishedprocess->waitingtime, TA, WTA);
    processCount--;
  }
  else if (algorithm == 1)
  {
    process *finishedprocess = runningProcess;
    printf("Process ID = %d Fininshed at time = %d\n", finishedprocess->id, getClk());
    int TA = getClk() - finishedprocess->arrivaltime;
  float WTA = (float)TA / (float)finishedprocess->runningtime;
  fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), finishedprocess->id, "finished", finishedprocess->arrivaltime, finishedprocess->runningtime, 0, finishedprocess->waitingtime, TA, WTA);
      allWTA[processCount-1]=WTA;
  totalWTA += WTA;

  totalWaitingTime += finishedprocess->waitingtime;
totalRT += finishedprocess->runningtime;
    // Remove the finished process from the queue
    PQremove(PQ, finishedprocess);
    free(finishedprocess);
    runningProcess = NULL;
    processCount--;
  }
  else
  {
    // when a process fnishes it should notify the scheduler on termination, the scheduler does NOT terminate the process.
    // the scheduler should remove the process from the queue and free its memory
    flag = 0;
  }
  signal(SIGUSR1, finishedPhandler);
}

///////////////////////////////////////////

void RRScheduler(int quantum)
{
  Q = createQueue();
  printf("Queue created with quantum = %d\n", quantum);
  int counter = 0;
  while (true)
  {
    if (!isEmpty(Q))
    {
      runningProcess = dequeue(Q);

      int remainingtime = runningProcess->remainingtime;
      int runtime;

      if (quantum < remainingtime)
        runtime = quantum;
      else
        runtime = remainingtime;
      if (runningProcess->remainingtime == runningProcess->runningtime)
      {
        runningProcess->starttime = getClk();
        runningProcess->waitingtime = runningProcess->starttime - (runningProcess->arrivaltime);
      fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d\n",getClk(), runningProcess->id, "started", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      }
      else
      {
        int tempTime = getClk()-runningProcess->laststoptime;
        runningProcess->waitingtime+=tempTime;
      fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d\n",getClk(), runningProcess->id, "resumed", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      }
      kill(runningProcess->realPid, SIGCONT);
      
      sleep(runtime);
    }

    // This loop checks for the incoming processes, if there is no incoming processes, it will break and continue running the current process
    if (processCount > 0)
    {
      // int lastID = -1;
      while ((msgrcv(sch_msgq_id, &SCH_message, sizeof(SCH_message.arrivedProcess), getpid(), IPC_NOWAIT)) != -1)
      {
        // if (lastID == SCH_message.arrivedProcess.id)
        //   continue;

        printf("Received\n");
        process *newprocess = initProcess();
        normalQenqueue(Q, newprocess);

        // lastID = newprocess->id;
      }
    }

    if (runningProcess)
    {
      // when a process fnishes it should notify the scheduler on termination, the scheduler does NOT terminate the process.
      if (flag)
      {
      kill(runningProcess->realPid, SIGSTOP);
      fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d\n",getClk(), runningProcess->id, "stopped", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      runningProcess->laststoptime = getClk();
        normalQenqueue(Q, runningProcess);
      }
      else
      {
    int TA = getClk() - runningProcess->arrivaltime;
  float WTA = (float)TA / (float)runningProcess->runningtime;
  fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), runningProcess->id, "finished", runningProcess->arrivaltime, runningProcess->runningtime, 0, runningProcess->waitingtime, TA, WTA);
      allWTA[processCount-1]=WTA;
  totalWTA += WTA;
  totalWaitingTime += runningProcess->waitingtime;
totalRT += runningProcess->runningtime;
        free(runningProcess);
        processCount--;
        flag = 1;
        printf("Done cleaning\n");
      }
    }

    if (processCount == 0)
      break;
  }

  free(Q);
}

///////////////////////////////////////////

process *initProcess()
{
  process *newprocess = createProcess(SCH_message.arrivedProcess.id, SCH_message.arrivedProcess.priority,
                                      SCH_message.arrivedProcess.arrivaltime, SCH_message.arrivedProcess.runningtime);

  char runnungtimearg[20]; // a string containing the raunnumg time to be sent as argument to the forked process
  sprintf(runnungtimearg, "%d", newprocess->runningtime);

  char arrivaltime[20]; // same for arrival time (msh 3aref hn7tagha wla la)
  sprintf(arrivaltime, "%d", newprocess->arrivaltime);

  int pid = forkNewProcess(runnungtimearg, arrivaltime, newprocess->runningtime); // create a real process
  newprocess->realPid = pid;                                                      // set the real id of the forked process
  return newprocess;
}

void outputFile()
{

  float avgWTA = (float)totalWTA / currentProcessCount;
  float avgWaitingTime = (float)totalWaitingTime / currentProcessCount;
  float CPUutilization = (float)totalRT / getClk() * 100;
  float currentTime = 0, StandardDeviation = 0;
  for(int i =0; i < currentProcessCount; i++)
  {
      StandardDeviation += pow(allWTA[i] - avgWTA, 2);
  }
  StandardDeviation = sqrt(StandardDeviation / currentProcessCount);

  fprintf(schedulerPref, "CPU Utilization = %.2f%% \nAvg WTA = %.2f\nAvg WT = %.2f\nStd WTA = %.2f\n",
          CPUutilization, avgWTA, avgWaitingTime, StandardDeviation);
  printf("Output generated successfully\n");
}