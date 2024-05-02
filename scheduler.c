#include "DataStructures/PriorityQueue.h"


PriorityQueue* PQ = NULL;
Queue* Q = NULL;
Queue* finishedQueue = NULL;

// set to zero when it receives a termination signal from a process
int flag = 1;

int processCount; // to check if all processes finished or other processes were not sent yet
int algorithm;

key_t sch_key_id;
int sch_rec_val, sch_msgq_id;
struct msgbuff SCH_message;

int children_shmid;
int *children_shmaddr;


process *runningProcess = NULL;

/////// FUNCTIONS ////////

int forkNewProcess(char* runtime, char* arrivaltime, int run);
void getAlgorithm();
void connectWithGenerator();
void addProcess();
void finishedPhandler(int signum);
void sigtermhandler(int signum);
void RRScheduler(int quantum);
void processTerminated(int signum);


void STRN();
void STRNaddprocess();


int main(int argc, char *argv[])
{
    initClk();

    signal(SIGTERM, sigtermhandler); // to free the allocated memory
    signal(SIGUSR1, finishedPhandler); // to recieve that a process has finished its execution
    signal(SIGUSR2, processTerminated); // to handle the termination of a process
    algorithm = atoi(argv[1]);
    processCount = atoi(argv[2]);
    finishedQueue = createQueue();
    connectWithGenerator();
    getAlgorithm();
    
    //TODO implement the scheduler :)
  
     // create queue to recieve finished processes
    while (processCount > 0)
    {
      sch_rec_val = msgrcv(sch_msgq_id, &SCH_message, sizeof(SCH_message.arrivedProcess), getpid(), !IPC_NOWAIT);
      if (sch_rec_val != -1)
        addProcess ();

      if(!PQisEmpty(PQ) && runningProcess == NULL)
      {
        runningProcess = PQpeek(PQ);
        kill(runningProcess->realPid, SIGCONT);
      }
    }
    destroyClk(true);
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
      //    printf("RECIEVED A PROCESS\n");
        }
        if(!PQisEmpty(PQ) && runningProcess == NULL)
        {
          runningProcess = PQpeek(PQ);
       //   printf("Continue ID = %d Starting from time = %d\n\n", runningProcess->realPid, getClk());
          runningProcess->lastRunningClk = getClk();
          kill(runningProcess->realPid, SIGCONT);
        }
      }
  free(PQ);
}


void STRNaddprocess()
{

  process *newprocess = createProcess(SCH_message.arrivedProcess.id, SCH_message.arrivedProcess.priority,
  SCH_message.arrivedProcess.arrivaltime, SCH_message.arrivedProcess.runningtime);

  char runnungtimearg[20]; // a string containing the raunnumg time to be sent as argument to the forked process
  sprintf(runnungtimearg, "%d", newprocess->runningtime);

  char arrivaltime[20]; // same for arrival time (msh 3aref hn7tagha wla la)
  sprintf(arrivaltime, "%d", newprocess->arrivaltime);
 
  int pid = forkNewProcess(runnungtimearg, arrivaltime, newprocess->runningtime); // create a real process
  newprocess->realPid = pid;                             // set the real id of the forked process

  process * currentrunning = NULL;
  
  if (!PQisEmpty(PQ)) // if the queue is not empty check if we should stop the running process
  {
    currentrunning = PQpeek(PQ);
    if (currentrunning == runningProcess)
    {
    currentrunning->remainingtime = currentrunning->remainingtime - (getClk()-currentrunning->lastRunningClk);
    currentrunning->lastRunningClk = getClk();
    }
   // printf("Current running id : %d - remaining time = %d\n",currentrunning->id, currentrunning->remainingtime);
   // printf("new running id : %d - remaining time = %d\n",newprocess->id, newprocess->remainingtime);
    if (newprocess->runningtime < currentrunning->remainingtime)
    {

        kill (currentrunning->realPid, SIGTSTP);
        if (currentrunning == runningProcess)
        {
        currentrunning->remainingtime = currentrunning->remainingtime - (getClk()-currentrunning->lastRunningClk);
        currentrunning->lastRunningClk = getClk();
        }
      //  printf("NOW the remaining time of ID = %d is %d\n", currentrunning->id, currentrunning->remainingtime);
        kill (newprocess->realPid, SIGCONT);
        runningProcess = newprocess;
        runningProcess->lastRunningClk = getClk();
    }
  }
  else // the queue is empty
  {
     // printf("SENT SIGCONT TO PID = %d\n", newprocess->realPid);
      kill (newprocess->realPid, SIGCONT);  // if no processes using the cpu then run the arrived one
      runningProcess = newprocess;
      runningProcess->lastRunningClk = getClk();
  }
  STRNenqueue(PQ, newprocess, newprocess->remainingtime);

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

  kill (id, SIGTSTP); // stop the forked (except if the ready queue is empty) process untill its turn

  return id;
}

///////////////////////////////////////////

void getAlgorithm()
{
  switch (algorithm) 
    {
    case 1:
        printf ("You are in HPF mode\n");
        PQ = createPriorityQueue();
    break;
    case 2:
        printf ("You are in SRTN mode\n");
       // PQ = createPriorityQueue();
        STRN();
    break;
    case 3:
        printf ("You are in RR mode\n");
        Q = createQueue();
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

void addProcess()
{
  process *newprocess = createProcess(SCH_message.arrivedProcess.id, SCH_message.arrivedProcess.priority,
                                      SCH_message.arrivedProcess.arrivaltime, SCH_message.arrivedProcess.runningtime);

  char runnungtimearg[20]; // a string containing the raunnumg time to be sent as argument to the forked process
  sprintf(runnungtimearg, "%d", newprocess->runningtime);

  char arrivaltime[20]; // same for arrival time (msh 3aref hn7tagha wla la)
  sprintf(arrivaltime, "%d", newprocess->arrivaltime);

  int pid = forkNewProcess(runnungtimearg, arrivaltime, newprocess->runningtime); // create a real process
  newprocess->realPid = pid;                             // set the real id of the forked process

    process * currentrunning = NULL;
    switch (algorithm)
    {
        case 1:
          if (!PQisEmpty(PQ)) // if the queue is not empty check if we should stop the running process
          {
            currentrunning = PQpeek(PQ);
            if (newprocess->priority < currentrunning->priority)
            {
              kill (currentrunning->realPid, SIGSTOP);
              kill (newprocess->realPid, SIGCONT);
            }
          }
          PQenqueue(PQ, newprocess, newprocess->priority);
        break;

        case 2:
          if (!PQisEmpty(PQ)) // if the queue is not empty check if we should stop the running process
          {
            currentrunning = PQpeek(PQ);
            if (newprocess->runningtime < currentrunning->remainingtime)
            {
                kill (currentrunning->realPid, SIGTSTP);
                kill (newprocess->realPid, SIGCONT);
                runningProcess = newprocess;
            }
          }
          else // the queue is empty
          {
            kill (newprocess->realPid, SIGCONT);  // if no processes using the cpu then run the arrived one
            runningProcess = currentrunning;
          }
          STRNenqueue(PQ, newprocess, newprocess->remainingtime);
        break;

        case 3:
          normalQenqueue(Q, newprocess);
       
        break;
        }
}


void sigtermhandler(int signum)
{
  free(Q);
  free(PQ);
  kill(getpgrp(), SIGKILL);
  signal(SIGTERM, sigtermhandler);
}



void finishedPhandler(int signum)
{
process *finishedprocess = NULL;

if (algorithm == 1 || algorithm == 2)
{
  finishedprocess = PQdequeue(PQ);
  runningProcess = NULL;
  // sch_rec_val = msgrcv(sch_msgq_id, &SCH_message, sizeof(SCH_message.arrivedProcess), getpid(), IPC_NOWAIT);
  // if (sch_rec_val != -1)
  //    addProcess ();
  // if (!PQisEmpty(PQ))
  // {
  //    printf("I will continue process id = %d\n", PQpeek(PQ)->realPid);
  //    kill (PQpeek(PQ)->realPid, SIGCONT); // start executing the next process
  // }
} 
else
{
  finishedprocess = dequeue(Q);
  if (!isEmpty(Q))
    kill (peek(Q)->realPid, SIGCONT); // start executing the next process
}
printf("Process ID = %d Fininshed at time = %d\n", finishedprocess->realPid, getClk());

free(finishedprocess);
processCount--;
// if (processCount == 0)
// {
//   kill(getppid(), SIGINT); // if all processes are done then close the program
// }
signal(SIGUSR1, finishedPhandler);
}

///////////////////////////////////////////

void RRScheduler(int quantum)
{
  // when a process fnishes it should notify the scheduler on termination, the scheduler does NOT terminate the process.

  runningProcess = dequeue(Q);

  int reamingtime = runningProcess->runningtime;

  while (!isEmpty(Q))
  {
    int runtime;
    if (quantum < reamingtime)
      runtime = quantum;
    else
      runtime = reamingtime;

    kill(runningProcess->realPid, SIGCONT);
    sleep(runtime);

    if (flag)
    {
      kill(runningProcess->realPid, SIGSTOP);
      normalQenqueue(Q, runningProcess);
    }
    else
    {
      free(runningProcess);
      flag = 1;
    }

  }

}

///////////////////////////////////////////

void processTerminated(int signum)
{
  // when a process fnishes it should notify the scheduler on termination, the scheduler does NOT terminate the process.
  // the scheduler should remove the process from the queue and free its memory

  flag = 0;
  
}