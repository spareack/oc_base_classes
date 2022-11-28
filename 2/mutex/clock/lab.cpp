#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


typedef struct 
{
    int flag;
    char sym;
    pthread_mutex_t *mutex;
} targs;

void* proc1(void *arg)
{
    printf("proc1 started\n");
    targs *args = (targs*) arg;

    while(args->flag != 1)
    {
		struct timespec tp;
		do {
			clock_gettime(CLOCK_REALTIME, &tp);
			tp.tv_sec += 1;
		} while(pthread_mutex_timedlock(args->mutex, &tp));
		
		for(int i = 0; i < 5; i += 1)
		{
			putchar(args->sym);
	        fflush(stdout);
			sleep(1);
		}
		pthread_mutex_unlock(args->mutex);
		sleep(1);
    }
    printf("proc1 finished\n");
    pthread_exit((void*)1);
}

void* proc2(void *arg)
{
    printf("proc2 started\n");
    targs *args = (targs*) arg;
    
    while(args->flag != 1)
    {
		struct timespec tp;
		do {
			clock_gettime(CLOCK_REALTIME, &tp);
			tp.tv_sec += 1;
		} while(pthread_mutex_timedlock(args->mutex, &tp));
		
		for(int i = 0; i < 5; i += 1)
		{
			putchar(args->sym);
	        fflush(stdout);
			sleep(1);
		}
		pthread_mutex_unlock(args->mutex);
		sleep(1);
    }
    printf("proc2 finished\n");
    pthread_exit((void*)2);
}

int main()
{
    printf("programm started\n");
    pthread_t thread1, thread2;
    
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);

    targs arg1, arg2;
    arg1.flag = 0; arg1.sym = '1'; arg1.mutex = &mutex;
    arg2.flag = 0; arg2.sym = '2'; arg2.mutex = &mutex;

    pthread_create(&thread1, NULL, proc1, &arg1);
    pthread_create(&thread2, NULL, proc2, &arg2);
    
    printf("waiting press button...\n");
    getchar();
    printf("button pressed\n");
    
    arg1.flag = 1; arg2.flag = 1;
    int *exitcode;

    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);
    pthread_join(thread2, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);
    
	pthread_mutex_destroy(&mutex);
    printf("programm finished\n");
    return 0;
}
