#include "types.h"        
#include "param.h"        
#include "memlayout.h"    
#include "mmu.h"          
#include "x86.h"          
#include "proc.h"         
#include "defs.h"         
#include "spinlock.h"
#include "semaphore.h"

struct semaphore sem_table[NSEM];

void
sema_init(int index, int val) {
  if(index < 0 || index >= NSEM) return;
  struct semaphore *s = &sem_table[index];
  initlock(&s->lock, "sema");
  s->value = val;
  s->head = s->tail = 0;
}

void
sema_wait(int index) {
  if(index < 0 || index >= NSEM) return;
  struct semaphore *s = &sem_table[index];

  acquire(&s->lock);
  s->value--;
  if(s->value < 0){
    s->queue[s->tail++] = myproc();
    sleep(myproc(), &s->lock);
  }
  release(&s->lock);
}


void
sema_signal(int index) {
  if(index < 0 || index >= NSEM) return;
  struct semaphore *s = &sem_table[index];

  acquire(&s->lock);
  s->value++;
  if(s->value <= 0 && s->head != s->tail){
    struct proc *p = s->queue[s->head++];
    wakeup(p);
  }
  release(&s->lock);
}
