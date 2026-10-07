
#ifndef THREAD_INCLUDED
#define THREAD_INCLUDED

#include <stddef.h>
#include <stdint.h>

typedef struct thread{
        int *sptr;
        int tid;
}Thread_t;

extern void Thread_init(void);
extern int Thread_new(int func(void *), void *args, size_t nbytes, ...);
extern void Thread_exit(int code);
extern int Thread_self(void);
extern int Thread_join(int tid);
extern void Thread_pause(void);

#endif
