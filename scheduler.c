#include "DataStructures/PriorityQueue.h"
#include <sys/file.h>
#include <math.h>

PriorityQueue *PQ = NULL;
Queue *Q = NULL;
Queue *finishedQueue = NULL;

    int TA;
    int WTA;
Queue *allWTA = NULL;

// set to zero when it receives a termination signal from a process
int flag = 1;
int quantum;

int processCount; // to check if all processes finished or other processes were not sent yet
int algorithm;

key_t sch_key_id;
int sch_rec_val, sch_msgq_id;
struct msgbuff SCH_message;

int children_shmid;
int *children_shmaddr;

int totalWaitingTime = 0;
int totalWTA = 0;
int totalRT = 0;

FILE *schedulerLog , *schedulerPref;

// Queue *allWTA = NULL;

process *runningProcess = NULL;

/////// FUNCTIONS ////////

int forkNewProcess(char *runtime, char *arrivaltime, int run);
void getAlgorithm();
void connectWithGenerator();
void finishedPhandler(int signum);
void sigtermhandler(int signum);
void RRScheduler(int quantum);
// void processTerminated(int signum);
process *initProcess();
void outputFileinit();
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

  signal(SIGTERM, sigtermhandler);    // to free the allocated memory
  signal(SIGUSR1, finishedPhandler);  // to recieve that a process has finished its execution
  // signal(SIGUSR2, processTerminated); // to handle the termination of a process
  algorithm = atoi(argv[1]);
  processCount = atoi(argv[2]);
  quantum = atoi(argv[3]);
  finishedQueue = createQueue();
  connectWithGenerator();

  getAlgorithm();

  allWTA = createQueue();

  // TODO implement the scheduler :)

  // create queue to recieve finished processes
  // while (processCount > 0)
  // {
  //   sch_rec_val = msgrcv(sch_msgq_id, &SCH_message, sizeof(SCH_message.arrivedProcess), getpid(), !IPC_NOWAIT);
  //   if (sch_rec_val != -1){

  //     addProcess();
  //     totalRunningTime+=SCH_message.arrivedProcess.runningtime;
  //   }
  //   if (!PQisEmpty(PQ) && runningProcess == NULL)
  //   {
  //     runningProcess = PQpeek(PQ);
  //     kill(runningProcess->realPid, SIGCONT);
  //   }
  // }
  outputFile();
  destroyClk(true);
  kill(getppid(), SIGINT);
  return 0;
}

void STRN()
{
  PQ = createPriorityQueue();
  int prev = getClk();
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
        printProcessState(schedulerLog, getClk(), runningProcess->id, "started", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      }
      else
      {
        int tempTime = getClk()-runningProcess->laststoptime;
        runningProcess->waitingtime+=tempTime;
        printProcessState(schedulerLog, getClk(), runningProcess->id, "resumed", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
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
      printProcessState(schedulerLog, getClk(), runningProcess->id, "stopped", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      runningProcess->laststoptime = getClk();
      runningProcess = newprocess;
      kill(runningProcess->realPid, SIGCONT);
      runningProcess->lastRunningClk = getClk();
      if (runningProcess->remainingtime == runningProcess->runningtime)
      {
        runningProcess->starttime = getClk();
        runningProcess->waitingtime = runningProcess->starttime - (runningProcess->arrivaltime);
        printProcessState(schedulerLog, getClk(), runningProcess->id, "started", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      }
      else
      {
        int tempTime = getClk()-runningProcess->laststoptime;
        runningProcess->waitingtime+=tempTime;
        printProcessState(schedulerLog, getClk(), runningProcess->id, "resumed", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
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
      if (runningProcess->remainingtime == runningProcess->runningtime)
      {
        runningProcess->starttime = getClk();
        runningProcess->waitingtime = runningProcess->starttime - (runningProcess->arrivaltime);
        printProcessState(schedulerLog, getClk(), runningProcess->id, "started", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      }
      else
      {
        int tempTime = getClk()-runningProcess->laststoptime;
        runningProcess->waitingtime+=tempTime;
        printProcessState(schedulerLog, getClk(), runningProcess->id, "resumed", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      }
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
  // if (runningProcess == NULL)
  // {
  //   kill(newprocess->realPid, SIGCONT);
  //   runningProcess = newprocess;
  // }
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
    // PQ = createPriorityQueue();
    HPF();
    break;
  case 2:
    printf("You are in SRTN mode\n");
    // PQ = createPriorityQueue();
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
  if (algorithm != 3)
  {
    process *finishedprocess = NULL;
    runningProcess = NULL;
    finishedprocess = PQdequeue(PQ);
    printf("Process ID = %d Fininshed at time = %d\n", finishedprocess->id, getClk());
  schedulerLog = fopen("scheduler.log", "w");
  if (schedulerLog == NULL)
  {
    perror("Error opening log file");
    exit(-1);
  }
  printProcessFinish(schedulerLog, getClk(), finishedprocess->id, "finished", finishedprocess->arrivaltime, finishedprocess->runningtime - (finishedprocess->remainingtime), finishedprocess->remainingtime, finishedprocess->waitingtime);
  
    free(finishedprocess);
    processCount--;
  }
  else
  {
    // when a process fnishes it should notify the scheduler on termination, the scheduler does NOT terminate the process.
    // the scheduler should remove the process from the queue and free its memory
    flag = 1;
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

      kill(runningProcess->realPid, SIGCONT);
      if (runningProcess->remainingtime == runningProcess->runningtime)
      {
        runningProcess->starttime = getClk();
        runningProcess->waitingtime = runningProcess->starttime - (runningProcess->arrivaltime);
        printProcessState(schedulerLog, getClk(), runningProcess->id, "started", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      }
      else
      {
        int tempTime = getClk()-runningProcess->laststoptime;
        runningProcess->waitingtime+=tempTime;
        printProcessState(schedulerLog, getClk(), runningProcess->id, "resumed", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      }
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
        printProcessState(schedulerLog, getClk(), runningProcess->id, "stopped", runningProcess->arrivaltime, runningProcess->runningtime - (runningProcess->remainingtime), runningProcess->remainingtime, runningProcess->waitingtime);
      runningProcess->laststoptime = getClk();
      normalQenqueue(Q, runningProcess);
      }
      else
      {
        free(runningProcess);
        processCount--;
        flag = 1;
        // printf("Done cleaning\n");
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
    schedulerPref = fopen("scheduler.pref", "w");

    float currentProcessCount = countQueue(allWTA);
    float avgWTA = (float)totalWTA / currentProcessCount;
    float avgWaitingTime = (float)totalWaitingTime / currentProcessCount;
    float CPUutilization = (float)totalRT / getClk() * 100;
    int currentTime = 0, StandardDeviation = 0;
    while (!isEmpty(allWTA))
    {
        currentTime = dequeue(allWTA);
        StandardDeviation += pow(currentTime - avgWTA, 2);
    }
    StandardDeviation = sqrt(StandardDeviation / currentProcessCount);

    schedulerPref = fopen("scheduler.pref", "w");
    if (schedulerPref == NULL)
    {
        perror("Error opening pref file");
        exit(-1);
    }

    fprintf(schedulerPref, "CPU Utilization = %.2f%% \nAvg WTA = %.2f\nAvg WT = %.2f\nStd WTA = %.2f\n",
            CPUutilization, avgWTA, avgWaitingTime, StandardDeviation);

    printf(" Output generated successfully\n");

    fclose(schedulerPref);
}

void printProcessState(FILE *schedulerLog, int time, int processID, char *state, int arrivalTime, int totalRunningTime, int remainingTime, int waitingTime)
{
  schedulerLog = fopen("scheduler.log", "a");
  if (schedulerLog == NULL)
  {
    perror("Error opening log file");
    exit(-1);
  }
  fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d\n",
          time, processID, state, arrivalTime, totalRunningTime, remainingTime, waitingTime);
  fclose("scheduler.log");
}


///////////////////////////////////////////

// void processTerminated(int signum)
// {
//   // when a process fnishes it should notify the scheduler on termination, the scheduler does NOT terminate the process.
//   // the scheduler should remove the process from the queue and free its memory

//   flag = 0;
// }

void printProcessFinish(FILE *schedulerLog, int time, int processID, char *state, int arrivalTime, int totalRunningTime, int remainingTime, int waitingTime)
{
    schedulerLog = fopen("scheduler.log", "a");

    int TA = time - arrivalTime;
    float WTA = (float)TA / (float)totalRunningTime;

    fprintf(schedulerLog, "#At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %.2f\n",
            time, processID, state, arrivalTime, totalRunningTime, remainingTime, waitingTime, TA, WTA);
        fclose("scheduler.log");
}