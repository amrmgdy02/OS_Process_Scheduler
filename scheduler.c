#include "DataStructures/PriorityQueue.h"



PriorityQueue* PQ;





// void initializeSRTN (){

//     PQ = createPriorityQueue();

// }



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
key_t SCH_key_id;
int SCH_rec_val, SCH_msgq_id;

struct msgbuff SCH_message;

void recivehandler (int signum);


int main(int argc, char * argv[])
{
   // initClk();
   // getClk();
    //TODO implement the scheduler :)
    
    PQ = createPriorityQueue();
    signal (SIGUSR1, recivehandler);
     

    SCH_key_id = ftok("keyfile", 65);               //create unique key
    SCH_msgq_id = msgget(10000, 0666 | IPC_CREAT); //create message queue and return id

    if (SCH_msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    printf("Message Queue ID = %d\n", SCH_msgq_id);

    
    while (true);

    //upon termination release the clock resources.
   
  //  destroyClk(true);
    return 0;
}

void recivehandler (int signum) {

    printf("I entered'n");
    SCH_rec_val = msgrcv(SCH_msgq_id, &SCH_message, sizeof(SCH_message.arrivedProcess), 0, !IPC_NOWAIT);
     if (SCH_rec_val == -1)
             perror("Error in receive");
         else {
                process * newprocess = createProcess(SCH_message.arrivedProcess.id, SCH_message.arrivedProcess.priority,
                 SCH_message.arrivedProcess.arrivaltime, SCH_message.arrivedProcess.arrivaltime);
            
               // printf("\nReceived process id: %d\n", newprocess->id);
                PQenqueue(PQ, newprocess, newprocess->priority);
             }
             
    printf("Priority queue now is: \n");    
    PQdisplay(PQ);
   
    signal (SIGUSR1, recivehandler);
}

