#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "param.h"

#define NSEM 6 
    
struct semaphore {
  int value;
  struct spinlock lock;
  struct proc *queue[NPROC];
  int head, tail;
};

void sema_init(int index, int val);
void sema_wait(int index);
void sema_signal(int index);

#endif
