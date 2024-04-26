#include "../DataStructures/PriorityQueue.c"


PriorityQueue* PQ;


// Function to create a new process node
process* createProcess(int processID, int priority, int arrivaltime, int runningtime) {
    process *newProcess = (process*)malloc(sizeof(process));
    if (newProcess == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newProcess->id = processID;
    newProcess->priority = priority;
    newProcess->arrivaltime = arrivaltime;
    newProcess->runningtime = runningtime;
    newProcess->remainingtime = runningtime;

    return newProcess;
}



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



int main(int argc, char * argv[])
{
    initClk();
    getClk();
    //TODO implement the scheduler :)
    //upon termination release the clock resources.

    destroyClk(true);
    return 0;
}
