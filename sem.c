#include "sem.h"
#include <stddef.h>
#include "queue.h"
#include "thread_internal.h"

void Sem_init(Sem_T *s, int count) { 
        s->count = count; 
        s->queue = NULL; 
}
void Sem_wait(Sem_T *s) { 
        assert(s != NULL);
        if (s->count>0) {
                s->count--;      

        } else {
                if(s->queue == NULL){
                        Queue_t *q = (Queue_t *)s->queue;
                        q = initq();
                        if(enqueue(&q, current))
                }
                block();
        }
}
void Sem_signal(Sem_T *s) { (void)s; }
