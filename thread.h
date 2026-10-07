
#ifndef THREAD_INCLUDED
#define THREAD_INCLUDED

#include <stddef.h>

typedef struct Queue Queue_t;   /* forward declaration, avoids circular include */

typedef struct thread {
        int *sp;        //stack pointer
        int tid;        //thread id
        Queue_t *q;     //threads in queue when join is called
        int *startsp;
        void (*exitcode)(void *);
} Thread_t;

extern Thread_t *Thread_init(void);
extern int Thread_new(int func(void *), void *args, size_t nbytes, ...);
extern void Thread_exit(int code);
extern int Thread_self(void);
extern int Thread_join(int tid);
extern void Thread_pause(void);

#endif
