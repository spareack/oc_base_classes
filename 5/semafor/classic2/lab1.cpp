#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define SEM_NAME "/spec_sem"
#define SYMBOL '1'

sem_t *sem;
FILE *fp;
int flag = 0;

void intHandler(int count) 
{
    fclose(fp);
    sem_close(sem);
    sem_unlink(SEM_NAME);
    exit(1);
}

void* proc(void *arg)
{
    printf("proc started\n");
    fp = fopen("file.txt", "a");

    while(flag != 1) 
    {
        if (sem_wait(sem) != 0) perror("sem_wait");

        for(int i = 0; i < 10; i += 1) 
        {
            fputc(SYMBOL, fp);
            fflush(fp);
            putchar(SYMBOL);
	        fflush(stdout);
			sleep(1);
        }
        fputc('\n', fp);
        fflush(fp);
        printf("\n");
		if (sem_post(sem)) perror("sem_post");
        sleep(1);
    }
    fclose(fp);
    printf("proc finished\n");
    pthread_exit((void*)1);
}

int main()
{
    printf("programm started\n");
    signal(SIGINT, intHandler);

    sem = sem_open(SEM_NAME, O_CREAT, FILE_MODE, 1);
    pthread_t thread1;
    printf("thread created with status %d\n", pthread_create(&thread1, NULL, proc, NULL));

    printf("waiting press button...\n");
    getchar();
    printf("button pressed\n");
    flag = 1;

    int *exitcode;
    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);

    sem_close(sem);
    sem_unlink(SEM_NAME);
    printf("programm finished\n");
    return 0;
}
