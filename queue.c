#include <stdlib.h>
#include "queue.h"
#include <stdio.h>


Queue_t* initq(){
        Queue_t *q = malloc(sizeof(Queue_t));
        if(q==NULL){
                return NULL;
        }
        q->head = NULL;
        q->tail = NULL;
        q->count = 0;
        return q;
}
QNode* initnode(Thread_t *t){
        QNode *node = NULL;
        node = malloc(sizeof(QNode));
        if(node == NULL)        
                return NULL;
        node->t = t;
        node->next = NULL;
        return node;
}
int enqueue(Queue_t **q, Thread_t *t){
        QNode *node = initnode(t);
        if(node == NULL){
                printf("here\n");
                return failure;
        }
        if((*q)->count == 0){
                (*q)->head = node;
                (*q)->tail = node;
                (*q)->count++;
                return success;
        }
        else{
                (*q)->tail->next = node;
                (*q)->tail = node;
                (*q)->count++;
                return success;
        }
        printf("Here2\n");
        return failure;
}

int dequeue(Queue_t **q, Thread_t **t){
        if((*q) == NULL || (*q)->count <= 0){
                return failure;
        }
        QNode *node = (*q)->head;
        if(node->next == NULL){
                (*q)->head = NULL;
                (*q)->tail = NULL;
                (*q)->count = 0;
                (*t) = node->t;
                free(node);
                return success;
        }
        (*q)->head = node->next;
        (*q)->count--;
        (*t) = node->t;
        free(node);
        return success;        
}

int isEmptyq(Queue_t *q){
        return q->count == 0;
}