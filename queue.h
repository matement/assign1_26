#ifndef QUEUE_H
#define QUEUE_H

#include "thread.h"

typedef struct Node{
        Thread_t *thread;
        QNode *next;
}QNode;

typedef struct Queue{
        void* head;
        void* tail;
        QNode * node;

}Queue_t;

#endif