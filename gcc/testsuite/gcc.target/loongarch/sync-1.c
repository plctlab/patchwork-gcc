/* Test __sync_test_and_set in atomic_exchange */
/* { dg-do run } */
/* { dg-options "-lpthread -std=c11" } */

#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdlib.h>

#define NR_THREAD 16
#define NR_DATA 1000
#define ITER_COUNT 10000

static int _data[NR_DATA];
static char _lock;
static int _overcnt;

static inline void proc_yield(int cnt)
{
  __asm__ __volatile__("":::"memory");
}

static void unlock()
{
  return atomic_store_explicit(&_lock, 0, memory_order_seq_cst);
}

static int trylock()
{
  return atomic_exchange_explicit(&_lock, 1, memory_order_acquire) == 0;
}

static void lockslow()
{
  for (int i = 0;; i++) {
    if (i < 10)
      proc_yield(i);
    else
      sched_yield();
    if (atomic_load_explicit(&_lock, memory_order_relaxed) == 0
      && atomic_exchange_explicit(&_lock, 1, memory_order_acquire) == 0)
      return;
  }
}

static void lock()
{
  if (trylock())
    return;
  lockslow();
}

static void checkeq(int a, int b)
{
  if (a != b)
    __builtin_abort();
}

static void adddata()
{
  int i, v;
  lock();
  v = _data[0];
  for (i = 0; i < NR_DATA; i++) {
    checkeq(_data[i], v);
    _data[i]++;
  }
  unlock();
}

static void backoff()
{
  int i, data[NR_DATA] = {0};
  for (i = 0; i < NR_DATA; i++) {
    data[i]++;
    checkeq(data[i], 1);
  }
}

static void *write_mutex_thread(void *unused)
{
  int i;
  for (i = 0; i < ITER_COUNT; i++) {
    adddata();
    backoff();
  }
  atomic_fetch_add(&_overcnt, 1);
}

int main()
{
  int cnt;

  pthread_t threads[NR_THREAD];
  for (int i = 0; i < NR_THREAD; i++)
    pthread_create(&threads[i], 0, write_mutex_thread, NULL);
  for (int i = 0; i < NR_THREAD; i++)
    pthread_detach(threads[i]);
  while(cnt != NR_THREAD) {
    sched_yield();
    cnt = atomic_load(&_overcnt);
  }
  return 0;
}
