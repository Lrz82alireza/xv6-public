#include "types.h"
#include "defs.h"
#include "rwlock.h"
#include "semaphore.h"

struct rwlock rwlocks[NRWLOCKS];

void init_rwlock(struct rwlock *lock, int sem_mutex_index, int sem_writeblock_index) {
  lock->readcount = 0;
  lock->sem_mutex_index = sem_mutex_index;
  lock->sem_writeblock_index = sem_writeblock_index;
  lock->used = 1;  // 🔥 فعال‌سازی lock
}

void reader_acquire(struct rwlock *lock) {
  sema_wait(lock->sem_mutex_index);  
  lock->readcount++;
  if (lock->readcount == 1) {
    sema_wait(lock->sem_writeblock_index);  // اولین Reader قفل Writer رو می‌گیره
  }
  sema_signal(lock->sem_mutex_index);
}

void reader_release(struct rwlock *lock) {
  sema_wait(lock->sem_mutex_index);
  lock->readcount--;
  if (lock->readcount == 0) {
    sema_signal(lock->sem_writeblock_index);  // آخرین Reader قفل Writer رو آزاد می‌کنه
  }
  sema_signal(lock->sem_mutex_index);
}

void writer_acquire(struct rwlock *lock) {
  sema_wait(lock->sem_writeblock_index);  // Writer باید صبر کنه تا دسترسی پیدا کنه
}

void writer_release(struct rwlock *lock) {
  sema_signal(lock->sem_writeblock_index);  // Writer بعد از نوشتن قفل رو آزاد می‌کنه
}

int rwlock_alloc(void) {
  for (int i = 0; i < NRWLOCKS; i++) {
    if (!rwlocks[i].used) {
      int sem_mutex_index = i * 2;
      int sem_writeblock_index = i * 2 + 1;
      sema_init(sem_mutex_index, 1);
      sema_init(sem_writeblock_index, 1);
      init_rwlock(&rwlocks[i], sem_mutex_index, sem_writeblock_index);
      return i;
    }
  }
  return -1;  // ظرفیت قفل‌ها پر شده
}


static int shared_var = 0;

int run_rw_pattern(int lock_id, int pattern) {
  if (lock_id < 0 || lock_id >= NRWLOCKS)
    return -1;

  struct rwlock *lock = &rwlocks[lock_id];
  if (!lock->used)
    return -1;

  for (int i = 0; i < 32; i++) {
    int bit = (pattern >> i) & 1;

    if (bit == 0) {
      reader_acquire(lock);
      cprintf("[Reader %d] shared_var = %d\n", i, shared_var);
      reader_release(lock);
    } else {
      writer_acquire(lock);
      shared_var += 1;
      cprintf("[Writer %d] shared_var updated to %d\n", i, shared_var);
      writer_release(lock);
    }
  }

  return 0;
}
