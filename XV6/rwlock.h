// rwlock.h
#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#include "semaphore.h"  // اگر semaphore ساختید اینو بذار، یا خودت تعریف کن

struct rwlock {
  struct semaphore mutex;         // محافظ readcount
  struct semaphore writeblock;    // محافظ writers
  int readcount;                  // شمارنده‌ی readerهای فعال
};

void init_rwlock(struct rwlock *lock);
void reader_acquire(struct rwlock *lock);
void reader_release(struct rwlock *lock);
void writer_acquire(struct rwlock *lock);
void writer_release(struct rwlock *lock);

#endif
