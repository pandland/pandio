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
#include "internal.h"

static size_t nthreads = 0;
static pd_thread_t *threads = NULL;

static pd_mutex_t mux;
static pd_cond_t cond;

static struct queue tasks = { .head = NULL, .tail = NULL };
static size_t ntasks = 0;


void pd_task_submit(pd_io_t *ctx, pd_task_t *task) {
    task->ctx = ctx;

    queue_init_node(&task->qnode);
    pd_mutex_lock(&mux);
    queue_push(&tasks, &task->qnode);
    ntasks++;
    pd_cond_signal(&cond);
    pd_mutex_unlock(&mux);
}


void* pd_threadpool_exec(void *arg) {
    while (true) {
        pd_mutex_lock(&mux);
        assert(ntasks >= 0);

        while (ntasks == 0) {
            pd_cond_wait(&cond, &mux);
        }

        struct queue_node *node = queue_pop(&tasks);
        pd_task_t *task = container_of(node, pd_task_t, qnode);
        ntasks--;

        pd_mutex_unlock(&mux);

        if (task->work)
            task->work(task);

        queue_init_node(&task->qnode);
        // TODO: push it to the "done queue" inside event loop
        //  pd_mutex_lock(&mux);
        //  queue_push(&task->ctx->tasks_done, &task->qnode);
        //  we need to signal somehow finished task
        //  pd_mutex_unlock(&mux);
    }

    return 0;
}


void pd_threadpool_init(size_t n) {
    // init only once for sure
    if (nthreads != 0)
        return;

    nthreads = (n > 0) ? n : PD_THREADPOOL_SIZE;
    threads = malloc(nthreads * sizeof(pd_thread_t));

    pd_mutex_init(&mux);
    pd_cond_init(&cond);

    for (int i = 0; i < nthreads; ++i) {
        pd_thread_create(&threads[i], pd_threadpool_exec, NULL);
    }
}
