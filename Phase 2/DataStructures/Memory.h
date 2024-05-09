#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#pragma once
#include "../Clock/headers.h"

typedef struct MemoryBlock {
    int start;
    int end;
    int size;
    int processId;
    bool isEmpty;
    int split;
    struct MemoryBlock* left;
    struct MemoryBlock* right;
    struct MemoryBlock* parent;
} MemoryBlock;

MemoryBlock* createMemoryBlock(int start, int size) {
    MemoryBlock* memBlock = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    memBlock->start = start;
    memBlock->end = start + size-1;
    memBlock->size = size;
    memBlock->split = 0;
    memBlock->processId = -1;
    memBlock->isEmpty = true;
    memBlock->left = NULL;
    memBlock->right = NULL;
    memBlock->parent = NULL;
    return memBlock;
}

bool addProcess(MemoryBlock* memBlock, process* process) {
    if (memBlock == NULL || memBlock->isEmpty == false || memBlock->size < process->memorySize) {
        return false;
    }
    if (memBlock->size == process->memorySize) {
        memBlock->processId = process->id;
        memBlock->isEmpty = false;
        int currentTime = getClk();
        printf("At time %d allocated %d bytes for process %d from %d to %d\n",currentTime , process->memorySize, process->id, memBlock->start, memBlock->end);
        return true;
    }
    int newSize = memBlock->size / 2;
    if(process->memorySize > newSize){       
        memBlock->processId = process->id;
        memBlock->isEmpty = false;
        while (memBlock->parent){
            memBlock->parent->split++;
            memBlock->parent = memBlock->parent->parent;
        }
        int currentTime = getClk();
        printf("At time %d allocated %d bytes for process %d from %d to %d\n",currentTime , process->memorySize, process->id, memBlock->start, memBlock->end);
        return true;
    }
    if (memBlock->left == NULL && memBlock->right == NULL) {
        memBlock->left = createMemoryBlock(memBlock->start, newSize);
        memBlock->left->parent = memBlock;
        memBlock->right = createMemoryBlock(memBlock->start + newSize,newSize);
        memBlock->right->parent = memBlock;
    }
    if(addProcess(memBlock->left, process)){
        return true;
    }else if(addProcess(memBlock->right, process)){
        return true;
    }
    return false;
}

void freeMemory(MemoryBlock* memBlock, int processId) {
    if (memBlock == NULL) return;

    if (memBlock->processId == processId) {
        memBlock->processId = -1;
        memBlock->isEmpty = true;
        while (memBlock->parent) {
            memBlock->parent->split--;
            if (memBlock->parent->left->isEmpty && memBlock->parent->right->isEmpty) {
                free(memBlock->parent->left);
                free(memBlock->parent->right);
                memBlock->parent->left = NULL;
                memBlock->parent->right = NULL;
            }
            memBlock = memBlock->parent;
        }
        return;
    }

    freeMemory(memBlock->left, processId);
    freeMemory(memBlock->right, processId);
}
