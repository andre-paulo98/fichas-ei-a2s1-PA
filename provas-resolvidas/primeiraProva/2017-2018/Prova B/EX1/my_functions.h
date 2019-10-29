#ifndef MY_FUNCTION_H
#define MY_FUNCTION_H

void thread_create(pthread_t *tid, void * (func)(void *), void *params);
void thread_join(pthread_t tid);

void mutex_init(pthread_mutex_t *mutex);
void mutex_destroy(pthread_mutex_t *mutex);

int mutex_lock(pthread_mutex_t *mutex);
int mutex_unlock(pthread_mutex_t *mutex);

void cond_ini(pthread_cond_t *cond);
void cond_destroy(pthread_cond_t *cond);

int cond_wait(pthread_cond_t *cond_wait, pthread_mutex_t *mutex);
int cond_signal(pthread_cond_t *cond_wait);
int cond_broadcast(pthread_cond_t *cond_wait);



#endif