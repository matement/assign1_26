#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include "thread.h"

enum retstate{
        success = 0,
        failure = -1
};


typedef struct Node {
        Thread_t *t;
        struct Node *next;
} QNode;

struct Queue {
        QNode *head;
        QNode *tail;
        int count;
};

Queue_t *initq();
QNode *initnode();
int enqueue(Queue_t **q, Thread_t *t);
int dequeue(Queue_t **q, Thread_t **t);

#endif
