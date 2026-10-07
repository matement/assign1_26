#include "sem.h"
#include <stddef.h>


/* dummy stubs so the project links; replace with the real implementation */
void Sem_init(Sem_T *s, int count) { s->count = count; s->queue = NULL; }
void Sem_wait(Sem_T *s) { (void)s; }
void Sem_signal(Sem_T *s) { (void)s; }
