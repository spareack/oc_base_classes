#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>


typedef struct 
{
    int flag;
    char sym;
    sem_t *sem;
} targs;


void* proc1(void *arg)
{
    printf("proc1 started\n");
    targs *args = (targs*) arg;

    while(args->flag != 1)
    {
        if (sem_wait(args->sem) != 0) perror("sem_wait");
    	
		for(int i = 0; i < 5; i += 1)
		{
			putchar(args->sym);
	        fflush(stdout);
			sleep(1);
		}
		sem_post(args->sem);
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
        if (sem_wait(args->sem) != 0) perror("sem_wait");
    	
		for(int i = 0; i < 5; i += 1)
		{
			putchar(args->sym);
	        fflush(stdout);
			sleep(1);
		}
		sem_post(args->sem);
		sleep(1);
    }
    printf("proc2 finished\n");
    pthread_exit((void*)2);
}

int main()
{
    printf("programm started\n");
    pthread_t thread1, thread2;
    
	sem_t  sem;
	if (sem_init(&sem, 0, 0) != 0) perror("sem_init");

    targs arg1, arg2;
    arg1.flag = 0; arg1.sym = '1'; arg1.sem = &sem;
    arg2.flag = 0; arg2.sym = '2'; arg2.sem = &sem;

    printf("%d", pthread_create(&thread1, NULL, proc1, &arg1));
    printf("%d", pthread_create(&thread2, NULL, proc2, &arg2));
    
    printf("waiting press button...\n");
    getchar();
    printf("button pressed\n");
    
    arg1.flag = 1; arg2.flag = 1;
    int *exitcode;

    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);
    pthread_join(thread2, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);
    
	sem_destroy(&sem);
    printf("programm finished\n");
    return 0;
}
