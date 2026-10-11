#include <stdio.h>
#include "thread.h"

static int other(void *arg) {
    Thread_join(1);          /* waits on main */
    return 0;
}
int main(void) {
    Thread_init();
    int t = Thread_new(other, NULL, 0);
    Thread_join(t);          /* waits on thread 2 */
    return 0;
}