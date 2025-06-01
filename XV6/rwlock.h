// rwlock.h
#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#define NRWLOCKS 4  // تعداد مجاز قفل‌ها

struct rwlock {
  int readcount;
  int sem_mutex_index;
  int sem_writeblock_index;
  int used;  // 🔥 اضافه شد: نشان‌دهنده فعال بودن این قفل
};

// مقداردهی اولیه rwlock
void init_rwlock(struct rwlock *lock, int sem_mutex_index, int sem_writeblock_index);

// عملیات Reader
void reader_acquire(struct rwlock *lock);
void reader_release(struct rwlock *lock);

// عملیات Writer
void writer_acquire(struct rwlock *lock);
void writer_release(struct rwlock *lock);

// درخواست قفل جدید
int rwlock_alloc(void);

#endif
