#include <stdio.h>
#include "thread.h"

static int worker(void *arg) {
    int n = *(int *)arg;
    for (int i = 0; i < 3; i++) { printf("worker %d round %d\n", Thread_self(), i); Thread_pause(); }
    return n;
}
int main(void) {
    Thread_init();
    int a = 10, b = 20;
    int t1 = Thread_new(worker, &a, sizeof a);
    int t2 = Thread_new(worker, &b, sizeof b);
    printf("join %d -> %d\n", t1, Thread_join(t1));   /* expect 10 */
    printf("join %d -> %d\n", t2, Thread_join(t2));   /* expect 20 */
    printf("join 99 -> %d\n", Thread_join(99));       /* expect -1 */
    Thread_join(0);
    Thread_exit(0);
}