#ifndef MY_FUNCTION_H
#define MY_FUNCTION_H

void thread_create(pthread_t *tid, void * (func)(void *), void *params);
void thread_join(pthread_t tid);

void mutex_init(pthread_mutex_t *mutex);
void mutex_destroy(pthread_mutex_t *mutex);

int mutex_lock(pthread_mutex_t *mutex);
int mutex_unlock(pthread_mutex_t *mutex);

void cond_init(pthread_cond_t *cond);
void cond_destroy(pthread_cond_t *cond);

int cond_wait(pthread_cond_t *cond_wait, pthread_mutex_t *mutex);
int cond_signal(pthread_cond_t *cond_wait);
int cond_broadcast(pthread_cond_t *cond_wait);

void signal_action(int signal, struct sigaction *act);


FILE * c_fopen(char *path, char* mode);
void c_fclose(FILE *fp);
void c_fseek(FILE *fp, int offset, int position);




#endif