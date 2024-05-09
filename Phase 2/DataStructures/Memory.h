#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../Clock/headers.h"

#define MEMORY_SIZE 1024

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

MemoryBlock* createMemoryTree(int start, int size) {
    MemoryBlock* root = createMemoryBlock(start, size);
    if (size > 1) {
        int newSize = size / 2;
        root->left = createMemoryTree(start, newSize);
        root->left->parent = root;
        root->right = createMemoryTree(newSize, size);
        root->right->parent = root;
    }
    return root;
}

MemoryBlock* createMemoryBlock(int start, int size) {
    MemoryBlock* memBlock = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    memBlock->start = start;
    memBlock->end = size-1;
    memBlock->size = size-start;
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
        return true;
    }
    if(addProcess(memBlock->left, process)){
        return true;
    }else if(addProcess(memBlock->right, process)){
        return true;
    }else if (memBlock-> split == 0){
        memBlock->processId = process->id;
        memBlock->isEmpty = false;
        while (memBlock->parent){
            memBlock->parent->split++;
            memBlock->parent = memBlock->parent->parent;
        }
        return true;
    }
    return false;
}

void freeMemory(MemoryBlock* memBlock, int processId) {
    if (memBlock == NULL) return;
    if (memBlock->processId == processId) {
        memBlock->processId = -1;
        memBlock->isEmpty = true;
        while (memBlock->parent){
            memBlock->parent->split--;
            memBlock->parent = memBlock->parent->parent;
        }
        return;
    }
    freeMemory(memBlock->left, processId);
    freeMemory(memBlock->right, processId);
}
