#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>


typedef struct 
{
    int flag;
    char sym;
    pthread_mutex_t *mutex;
} targs;


void* proc1(void *arg)
{
    printf("\nThread 1 has started\n");
    targs *args = (targs*) arg;

    struct timespec ts1, ts2;

    while(args->flag != 1)
    {
    	while(pthread_mutex_trylock(args->mutex) != 0)
        {
            clock_gettime(CLOCK_REALTIME, &ts1);
            sleep(0.1);
            clock_gettime(CLOCK_REALTIME, &ts2);
            printf("before sleep: %ld.%ld", ts1.tv_sec, ts1.tv_nsec / 100000000);
            printf(" | ");
            printf("after sleep: %ld.%ld", ts2.tv_sec, ts2.tv_nsec / 100000000);

            if ((ts2.tv_nsec - ts1.tv_nsec) / 100000000 != 1)
                printf(" | incorrect sleep time:  %ld.%ld", ts2.tv_sec - ts1.tv_sec, (ts2.tv_nsec - ts1.tv_nsec) / 100000000);
            else 
                printf(" | correct sleep time: %ld.%ld", ts2.tv_sec - ts1.tv_sec, (ts2.tv_nsec - ts1.tv_nsec) / 100000000);
            printf("\n");
        }

		for(int i = 0; i < 5; i += 1)
		{
			putchar(args->sym);
	        fflush(stdout);
			sleep(1);
		}
		pthread_mutex_unlock(args->mutex);
		sleep(1);
    }
    printf("\nThread 1 finished\n");
    pthread_exit((void*)1);
}

void* proc2(void *arg)
{
    printf("\nThread 2 has started\n");
    targs *args = (targs*) arg;

    struct timespec ts1, ts2;
    
    while(args->flag != 1)
    {
    	while(pthread_mutex_trylock(args->mutex) != 0)
        {
            clock_gettime(CLOCK_REALTIME, &ts1);
            sleep(0.1);
            clock_gettime(CLOCK_REALTIME, &ts2);
            printf("before sleep: %ld.%ld", ts1.tv_sec, ts1.tv_nsec / 100000000);
            printf(" | ");
            printf("after sleep: %ld.%ld", ts2.tv_sec, ts2.tv_nsec / 100000000);

            if ((ts2.tv_nsec - ts1.tv_nsec) / 100000000 != 1)
                printf(" | incorrect sleep time:  %ld.%ld", ts2.tv_sec - ts1.tv_sec, (ts2.tv_nsec - ts1.tv_nsec) / 100000000);
            else 
                printf(" | correct sleep time: %ld.%ld", ts2.tv_sec - ts1.tv_sec, (ts2.tv_nsec - ts1.tv_nsec) / 100000000);
            printf("\n");
        }

		for(int i = 0; i < 5; i += 1)
		{
			putchar(args->sym);
	        fflush(stdout);
			sleep(1);
		}
		pthread_mutex_unlock(args->mutex);
		sleep(1);
    }
    printf("\nThread 2 finished\n");
    pthread_exit((void*)2);
}

int main()
{
    printf("\nThe program has started\n");
    pthread_t thread1, thread2;
    
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);

    targs arg1, arg2;
    arg1.flag = 0; arg1.sym = '1'; arg1.mutex = &mutex;
    arg2.flag = 0; arg2.sym = '2'; arg2.mutex = &mutex;

    pthread_create(&thread1, NULL, proc1, &arg1);
    pthread_create(&thread2, NULL, proc2, &arg2);
    
    printf("\nThe program is waiting for a keystroke\n");
    getchar();
    printf("\nKey pressed\n");
    
    arg1.flag = 1; arg2.flag = 1;
    int *exitcode;

    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);
    pthread_join(thread2, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);
    
	pthread_mutex_destroy(&mutex);
    printf("\nThe program finished\n");
    return 0;
}
