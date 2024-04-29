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

PriorityQueue* PQ;
Queue* Q;

int algorithm;

key_t sch_key_id;
int sch_rec_val, sch_msgq_id;
struct msgbuff SCH_message;


/////// FUNCTIONS ////////

int forkNewProcess ();
void getAlgorithm ();
void connectWithGenerator ();
void addProcess ();
void recivehandler (int signum);


int main(int argc, char * argv[])
{
    initClk();
    algorithm = atoi(argv[1]);

    connectWithGenerator();
    getAlgorithm();
    
    //TODO implement the scheduler :)

    while (true)
    {
    sch_rec_val = msgrcv(sch_msgq_id, &SCH_message, sizeof(SCH_message.arrivedProcess), getpid(), !IPC_NOWAIT);
    if (sch_rec_val == -1)
        perror("Error in receive");
    else 
    { 
        addProcess ();
    }
    }
    destroyClk(true);
    return 0;
}

int forkNewProcess ()
{
  int id = fork();
  if (id == -1)
  {
	   	perror("error in fork");
      exit(-1);
  }
  else if (id == 0)
  {
      if (execl("./process.out","process.out", NULL) == -1)
      {
          perror("execl: ");
          exit(1);
      }
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
    SCH_message.arrivedProcess.arrivaltime, SCH_message.arrivedProcess.arrivaltime);
    int pid = forkNewProcess(); // create a real process
    newprocess->id = pid; // set the real id of the forked process
    switch (algorithm)
    {
        case 3:
          normalQenqueue(Q, newprocess);
        //  printf("Queue now is: \n");    
        //  display(Q);
          break;
        default:
          PQenqueue(PQ, newprocess, newprocess->priority);
        //  printf("Priority queue now is: \n");    
        //  PQdisplay(PQ);
          break;
        }
}