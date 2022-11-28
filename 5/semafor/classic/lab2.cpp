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
#define sem_name "/spec_sem"

sem_t *sem;
FILE *fp;
int flag = 0;
char sym = '2';


void intHandler(int count) 
{
    fclose(fp);
    sem_close(sem);
    sem_unlink(sem_name);
    exit(1);
}

void getChar()
{
    int count = getchar();
    if (count != -1)
    {
        flag = 1;
        printf("button pressed\n");
    }
}

void writeChar(int count)
{
    fp = fopen("file.txt", "a");
    fputc(count, fp);
    fclose(fp);
}

int main()
{
    printf("programm started\n");
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    signal(SIGINT, intHandler);

    sem = sem_open(sem_name, O_CREAT, FILE_MODE, 1);
    printf("waiting press button...\n");

    while(flag != 1) 
    {
        if (sem_wait(sem) != 0) perror("sem_wait");

        for(int i = 0; i < 10; i += 1) 
        {
            writeChar(sym);
            putchar(sym);
	        fflush(stdout);
            getChar();
			sleep(1);
        }
        writeChar('\n');
        printf("\n");
		if (sem_post(sem)) perror("sem_post");
        sleep(1);
    }

    sem_close(sem);
    sem_unlink(sem_name);
    printf("programm finished\n");
    return 0;
}
