
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "debug.h"

#include "my_functions.h"

#define THREAD_CREATE_ERROR 10
#define THREAD_JOIN_ERROR 15

#define MUXTEX_INI_ERROR 11
#define MUXTEX_DESTROY_ERROR 16

#define COND_INI_ERROR 12
#define COND_DESTROY_ERROR 17

#define SIGNAL_SIGACTION_ERROR 20
#define SIGNAL_SIGACTION_ERROR 20

#define FILE_OPEN_ERROR 30
#define FILE_CLOSE_ERROR 31
#define FILE_SEEK_ERROR 32


void thread_create(pthread_t *tid, void * (func)(void *), void *params){
    if ((errno = pthread_create(tid, NULL, func, params)) != 0)
		ERROR(THREAD_CREATE_ERROR, "Erro ao criar Thread!\n");
}

void thread_join(pthread_t tid){
    if ((errno = pthread_join(tid, NULL)) != 0)
		ERROR(THREAD_JOIN_ERROR, "Erro aguardar pela Thread '%ld'!\n",tid);
}



void mutex_init(pthread_mutex_t *mutex){
    if ((errno = pthread_mutex_init(mutex, NULL)) != 0)
		ERROR(MUXTEX_INI_ERROR, "Erro ao iniciar o Mutex!\n");
}

void mutex_destroy(pthread_mutex_t *mutex){
    if ((errno = pthread_mutex_destroy(mutex)) != 0)
		ERROR(MUXTEX_DESTROY_ERROR, "Erro ao destruir o Mutex!\n");
}


int mutex_lock(pthread_mutex_t *mutex){
    if ((errno = pthread_mutex_lock(mutex)) != 0){
		WARNING("Erro ao bloquear o Mutex!\n");
		return 0;
    }
    return 1;
}
int mutex_unlock(pthread_mutex_t *mutex){
    if ((errno = pthread_mutex_unlock(mutex)) != 0){
		WARNING("Erro ao desbloquear o Mutex!\n");
		return 0;
    }
    return 1;
}


void cond_int(pthread_cond_t *cond){
    if ((errno = pthread_cond_init(cond, NULL)) != 0)
		ERROR(COND_INI_ERROR, "Erro ao iniciar a variavel de condição!\n");
}

void cond_destroy(pthread_cond_t *cond){
    if ((errno = pthread_cond_destroy(cond)) != 0)
		ERROR(COND_DESTROY_ERROR, "Erro ao destroir o variavel de condição!\n");
}

int cond_wait(pthread_cond_t *cond_wait, pthread_mutex_t *mutex){
    if ((errno = pthread_cond_wait(cond_wait, mutex) != 0)){
        WARNING("Erro ao aguardar pela variavel de condição!\n");
        return 0;
    }
    return 1;
}

int cond_signal(pthread_cond_t *cond_wait){
    if ((errno = pthread_cond_signal(cond_wait)) != 0){
        WARNING("Erro ao enviar sinal para variavel de condição!\n");
        return 0;
    }
    return 1;
}

int cond_broadcast(pthread_cond_t *cond_wait){
    if ((errno = pthread_cond_broadcast(cond_wait)) != 0){
        WARNING("Erro ao enviar broadcast para variavel de condição!\n");
        return 0;
    }
    return 1;
}

void signal_action(int signal, struct sigaction *act){
  if(sigaction(signal, act, NULL) < 0)
		ERROR(SIGNAL_SIGACTION_ERROR, "Erro ao subscrever o sinal '%s'",strsignal(signal));
}


FILE * c_fopen(char *path, char* mode){
    FILE *file;
    if ((file = fopen(path, mode)) == NULL)
		ERROR(FILE_OPEN_ERROR, "Não foi possível abrir o ficheiro '%s'\n",path);
    return file;
}

void c_fclose(FILE *fp){
    if (fclose(fp) != 0)
		ERROR(FILE_CLOSE_ERROR, "Não foi possível fechar o ficheiro!\n");
}

void c_fseek(FILE *fp, int offset, int position){
    if(fseek(fp, offset,position) != 0)
		ERROR(FILE_SEEK_ERROR, "Não foi possível posicionar o ponteiro no ficheiro!\n");
}