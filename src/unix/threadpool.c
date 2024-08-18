/* Copyright (c) Michał Dziuba
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

#include "../queue.h"

#define THREAD_POOL_SIZE 4

struct pnd_task {
  void (*func)(void*);
  void *args;
  struct queue_node qnode;
};

typedef struct pnd_task pnd_task_t;

static size_t nthreads = 0;
static pthread_t *threads = NULL;
static pthread_mutex_t mut;
static pthread_cond_t cond;
static struct queue tasks = { .head = NULL, .tail = NULL };
static size_t ntasks = 0;


void *pnd_work_exec(void *arg) {
  while (true) {
    pthread_mutex_lock(&mut);
    assert(ntasks >= 0);

    while (ntasks == 0) {
      pthread_cond_wait(&cond, &mut);
    }


    struct queue_node *node = queue_pop(&tasks);
    pnd_task_t *task = container_of(node, pnd_task_t, qnode);
    ntasks--;

    pthread_mutex_unlock(&mut);

    if (task->func)
      task->func(task->args);
    
    free(task);
  }
}


void pnd_work_submit(void (*func)(void*), void *args) {
  pnd_task_t *task = malloc(sizeof(pnd_task_t));

  task->func = func;
  task->args = args;

  queue_init_node(&task->qnode);
  pthread_mutex_lock(&mut);
  queue_push(&tasks, &task->qnode);
  ntasks++;

  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mut);
}


void pnd_workers_init() {
  nthreads = THREAD_POOL_SIZE;
  threads = malloc(nthreads * sizeof(pthread_t));

  printf("Thread pool size: %ld\n", nthreads);

  pthread_mutex_init(&mut, NULL);
  pthread_cond_init(&cond, NULL);

  for (int i = 0; i < nthreads; ++i) {
    pthread_create(&threads[i], NULL, pnd_work_exec, NULL);
  }
}


void pnd_workers_stop() {
  void *ret = NULL;

  for (int i = 0; i < nthreads; ++i) {
    pthread_join(threads[i], ret);
  }
}


// "long taks"
void test_task(void *args) {
  sleep(5);
  printf("Hello World\n");
}


int main() {
  pnd_workers_init();
  for (int i = 0; i < 4; ++i) {
    pnd_work_submit(test_task, NULL);
  }

  pnd_workers_stop();

  return 0;
}
