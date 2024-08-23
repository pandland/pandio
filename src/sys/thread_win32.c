#include "core.h"

void pd_mutex_init(pd_mutex_t *mux) {
    InitializeCriticalSection(mux);
}


void pd_mutex_lock(pd_mutex_t *mux) {
    EnterCriticalSection(mux);
}


void pd_mutex_unlock(pd_mutex_t *mux) {
    LeaveCriticalSection(mux);
}


void pd_mutex_destroy(pd_mutex_t *mux) {
    DeleteCriticalSection(mux);
}


void pd_cond_init(pd_cond_t *cond) {
    InitializeConditionVariable(cond);
}


void pd_cond_wait(pd_cond_t *cond, pd_mutex_t *mux) {
    SleepConditionVariableCS(cond, mux, INFINITE);
}


void pd_cond_signal(pd_cond_t *cond) {
    WakeConditionVariable(cond);
}


// On Windows it is not necessary.
void pd_cond_destroy(pd_cond_t *cond) {}
