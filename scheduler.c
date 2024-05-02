#include "DataStructures/PriorityQueue.h"






// void SRTN () {

//     int prevtime = getClk();
//     int diff = 0;

//     process* runningProcess = peek(PQ);

//     while (peek(PQ)) {

//         if ((diff = getClk() - prevtime) != 0) {

//             prevtime = getClk();
//             runningProcess->remainingtime -= diff;
            
//             if (runningProcess->remainingtime == 0){
                
//                 process* finished = dequeue(PQ);
//                 free(finished);
//             } 
//         }

//     }

// }

PriorityQueue* PQ = NULL;
Queue* Q = NULL;
Queue* finishedQueue = NULL;

int processCount; // to check if all processes finished or other processes were not sent yet
int algorithm;

key_t sch_key_id;
int sch_rec_val, sch_msgq_id;
struct msgbuff SCH_message;


/////// FUNCTIONS ////////

int forkNewProcess(char* runtime, char* arrivaltime);
void getAlgorithm();
void connectWithGenerator();
void addProcess();
void finishedPhandler(int signum);
void sigtermhandler(int signum);


int main(int argc, char * argv[])
{
    initClk();

    signal(SIGTERM, sigtermhandler); // to free the allocated memory
    signal(SIGUSR1, finishedPhandler); // to recieve that a process has finished its execution
    algorithm = atoi(argv[1]);
    processCount = atoi(argv[2]);

    connectWithGenerator();
    getAlgorithm();
    
    //TODO implement the scheduler :)
    finishedQueue = createQueue(); // create queue to recieve finished processes
    while (true)
    {
    sch_rec_val = msgrcv(sch_msgq_id, &SCH_message, sizeof(SCH_message.arrivedProcess), getpid(), !IPC_NOWAIT);
    if (sch_rec_val != -1)
       addProcess ();
    }
    destroyClk(true);
    return 0;
}

int forkNewProcess (char* runnungtime, char* arrivaltime)
{
  int id = fork();
  if (id == -1)
  {
	   	perror("error in fork");
      exit(-1);
  }
  else if (id == 0)
  {
      if (execl("./process.out","process.out", runnungtime, arrivaltime, NULL) == -1)
      {
          perror("execl: ");
          exit(1);
      }
  }

  if (((algorithm == 1 || algorithm == 2) && !PQisEmpty(PQ)) || (algorithm == 3 && !isEmpty(Q)))
  {
      kill (id, SIGSTOP); // stop the forked (except if the ready queue is empty) process untill its turn
  }
 
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
        PQ = createPriorityQueue();
    break;
    case 3:
        printf ("You are in RR mode\n");
        Q = createQueue();
    break;
    }
}

///////////////////////////////////////////

void connectWithGenerator ()
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

void addProcess ()
{
    process * newprocess = createProcess(SCH_message.arrivedProcess.id, SCH_message.arrivedProcess.priority,
    SCH_message.arrivedProcess.arrivaltime, SCH_message.arrivedProcess.runningtime);

    char runnungtimearg[20]; // a string containing the raunnumg time to be sent as argument to the forked process
    sprintf(runnungtimearg, "%d", newprocess->runningtime);

    char arrivaltime[20]; // same for arrival time (msh 3aref hn7tagha wla la)
    sprintf(arrivaltime, "%d", newprocess->arrivaltime);

    int pid = forkNewProcess(runnungtimearg, arrivaltime); // create a real process
    newprocess->realPid = pid; // set the real id of the forked process

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
                kill (currentrunning->realPid, SIGSTOP);
                kill (newprocess->realPid, SIGCONT);
            }
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
  if (!PQisEmpty(PQ))
    kill (PQpeek(PQ)->realPid, SIGCONT); // start executing the next process
}
else
{
  finishedprocess = dequeue(Q);
  if (!isEmpty(Q))
    kill (peek(Q)->realPid, SIGCONT); // start executing the next process
}

normalQenqueue(finishedQueue, finishedprocess);
processCount--;
if (processCount == 0)
{
  kill(getppid(), SIGINT); // if all processes are done then close the program
}
signal(SIGUSR1, finishedPhandler);
}