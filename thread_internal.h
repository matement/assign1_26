#ifndef THREAD_INTERNAL
#define THREAD_INTERNAL

extern Thread_t *current;
extern Queue_t *ready_queue;
void thread_block(void);

#endif