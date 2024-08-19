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

#include "threadpool.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include "poll.h"
#include <sys/eventfd.h>

static size_t nthreads = 0;
static pthread_t *threads = NULL;
static pthread_mutex_t mut;
static pthread_cond_t cond;
static struct queue tasks = { .head = NULL, .tail = NULL };
static size_t ntasks = 0;


void pnd_work_done_signal(pnd_io_t *ctx)
{
  int64_t u = 1;
  write(ctx->task_signal, &u, sizeof(int64_t));
}


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

    if (task->work)
      task->work(task);
    
    queue_init_node(&task->qnode); // reset qnode and later push to the tasks done queue
    
    pthread_mutex_lock(&mut);
    queue_push(&task->ctx->tasks_done, &task->qnode);

    pnd_work_done_signal(task->ctx);
    pthread_mutex_unlock(&mut);
  }
}


void pnd_work_done(pnd_io_t *ctx)
{
  struct queue tasks_done;
  queue_init(&tasks_done);

  pthread_mutex_lock(&mut);

  // copy data from synchronized queue to the local queue
  while (!queue_empty(&ctx->tasks_done)) {
    struct queue_node *node = queue_pop(&ctx->tasks_done);
    queue_push(&tasks_done, node);
  }

  pthread_mutex_unlock(&mut);
  
  // execute done callbacks in the main thread
  while (!queue_empty(&tasks_done)) {
    struct queue_node *node = queue_pop(&tasks_done);
    pnd_task_t *task = container_of(node, pnd_task_t, qnode);

    if (task->done)
      task->done(task);
  }
}


void pnd_work_done_io(pnd_event_t *ev, unsigned events) 
{
  if (events & PND_READABLE) {
    uint64_t u;
    if (read(ev->ctx->task_signal, &u, sizeof(uint64_t)) == sizeof(uint64_t))
      pnd_work_done(ev->ctx);
  }
}


void pnd_work_submit(pnd_io_t *ctx, pnd_task_t *task) 
{
  task->ctx = ctx;

  queue_init_node(&task->qnode);
  pthread_mutex_lock(&mut);
  queue_push(&tasks, &task->qnode);
  ntasks++;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mut);
}


void pnd_workers_init() 
{
  // init only once
  if (nthreads != 0)
    return;

  nthreads = THREAD_POOL_SIZE;
  threads = malloc(nthreads * sizeof(pthread_t));

  pthread_mutex_init(&mut, NULL);
  pthread_cond_init(&cond, NULL);

  for (int i = 0; i < nthreads; ++i) {
    pthread_create(&threads[i], NULL, pnd_work_exec, NULL);
  }
}
