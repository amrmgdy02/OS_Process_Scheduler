#include "Queue.h"


struct intQueueNode;
typedef struct intQueueNode* intQueueNodePtr;

typedef struct intQueueNode {
    int nodeProcess;
    intQueueNodePtr* next;
} intQueueNode;


typedef struct intQueue {
    intQueueNode* front;
} intQueue;


Queue* intcreateQueue() {
    Queue* pq = (Queue*)malloc(sizeof(Queue));
    if (pq == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    pq->front = NULL;
    return pq;
}

int intisEmpty (intQueue* q) {
    if (q->front == NULL)
        return 1;
    return 0;
}

void intnormalQenqueue(intQueue* pq, int newprocess) {
    // Create a new QueueNode
    intQueueNode* newNode = (intQueueNode*)malloc(sizeof(intQueueNode));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->nodeProcess = newprocess;

    if (pq->front == NULL) {
        newNode->next = pq->front;
        pq->front = newNode;
    } else {
        intQueueNode* current = pq->front;
        while (current->next != NULL) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}


int intdequeue(intQueue* pq) {
    if (pq->front == NULL) {
       // printf("Queue is empty\n");
        return NULL;
    }
    intQueueNode* temp = pq->front;
    pq->front = pq->front->next;
    int dequeuedProcess = temp->nodeProcess;
    temp = NULL;
    free(temp);
    return dequeuedProcess;
}




int intcountQueue(intQueue* queue) {
    int count = 0;
    intQueueNode* current = queue->front;
    while (current!= NULL) {
        count++;
        current = current->next;
    }
    return count;
}