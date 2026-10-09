
#ifndef THREAD_INCLUDED
#define THREAD_INCLUDED

#include <stddef.h>

typedef struct Queue Queue_t;   /* forward declaration, avoids circular include */
typedef struct thread Thread_t; /* opaque; defined in thread.c */

extern void Thread_init(void);
extern int Thread_new(int func(void *), void *args, size_t nbytes, ...);
extern void Thread_exit(int code);
extern int Thread_self(void);
extern int Thread_join(int tid);
extern void Thread_pause(void);

#endif
