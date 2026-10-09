#include <stdio.h>
#include <stdlib.h>
#include "queue.h"


int main(){
        Thread_init();
        printf("Thread is %d\n", Thread_self());
        return 0;
}