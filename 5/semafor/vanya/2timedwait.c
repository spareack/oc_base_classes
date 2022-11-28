#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>

typedef struct 
{
    int flag;
    char sym;
}targs;

typedef struct {
    time_t tv_sec;	
    long tv_nsec;	
}timespec;

sem_t semaphore;

void* proc1 (void *arg) {
    printf("\nproc1 started\n");
    targs *args = (targs*) arg;

    while(args->flag != 0){
        timespec tp;
        int waitResult = 1;
        while(waitResult != 0){
            clock_gettime(CLOCK_REALTIME, &tp);
            tp.tv_sec += 1;
            waitResult = sem_timedwait(&semaphore, &tp);
        }
        for(int i = 0; i < 5; i++){
            putchar(args->sym);
            fflush(stdout);
            sleep(1);
        }    
        sem_post(&semaphore);
        sleep(1);
    }

    printf("\nproc1 finished\n");
    pthread_exit((void*)1);
}

void* proc2 (void *arg) {
    printf("\nproc2 started\n");
    targs *args = (targs*) arg;

    while(args->flag != 0){
        timespec tp;
        int waitResult = 1;
        while(waitResult != 0){
            clock_gettime(CLOCK_REALTIME, &tp);
            tp.tv_sec += 1;
            waitResult = sem_timedwait(&semaphore, &tp);
        }
        for(int i = 0; i < 5; i++){
            putchar(args->sym);
            fflush(stdout);
            sleep(1);
        }    
        sem_post(&semaphore);
        sleep(1);
    }
    
    printf("\nproc2 finished\n");
    pthread_exit( (void*)2 );
}

int myGetChar() {
    printf("Press the button!\n");
    int symbol = getchar();
    printf("Button pressed!\n");
    return symbol;
}

int main () {
    printf("Programm started!\n");
    pthread_t id1, id2;
    targs arg1, arg2;
    int *exitcode1;
    int *exitcode2;
    arg1.flag = 1; arg1.sym = '1';
    arg2.flag = 1; arg2.sym = '2';
    sem_init(&semaphore, 0, 1);

    pthread_create(&id1, NULL, proc1, &arg1);
    pthread_create(&id2, NULL, proc2, &arg2);
    myGetChar();
    arg1.flag = 0;
    arg2.flag = 0;
    int result1, result2;
    pthread_join(id1, (void**)&exitcode1);
    pthread_join(id2, (void**)&exitcode2);
    sem_destroy(&semaphore);

    printf("Programm finished!\n");
    return 0;
}