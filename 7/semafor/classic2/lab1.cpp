#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>


int flag = 0;
sem_t *semRead;
sem_t *semWrite;
int fd;

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DATA_SIZE 25
#define PIPE_PATH "/tmp/spec_pipe"
pthread_t thread1, thread2;


void sig_handler(int count) 
{
    close(fd);
    unlink(PIPE_PATH);
    printf("lost connection with reader..\n");
    exit(1);
}

char *getSpecialValue()
{
    uid_t uid = getuid();
    struct passwd *pwuid = getpwuid(uid);
    return pwuid->pw_gecos;
}


void *proc_main_cycle(void *arg)
{
    char *data;
    while(flag != 1) 
    {
        data = getSpecialValue();
        write(fd, data, DATA_SIZE);
        printf("send data: %s\n", data);
        sleep(1);
    }
    printf("proc 'proc_main_cycle' finished\n");
    pthread_exit((void*)2);
}

void *proc_wait_connection(void *arg)
{
    printf("proc started\n");
    while(flag != 1) 
    {
        fd = open(PIPE_PATH, O_WRONLY | O_NONBLOCK);
        if (fd == -1)
        {
            if (errno == ENXIO)
            {
                perror("open");
                sleep(1);
            }
            else
            {
                perror("open");
                exit(1);
            }
        }
        else
        {
            printf("thread 'proc_main_cycle' created with status %d\n", pthread_create(&thread1, NULL, proc_main_cycle, NULL));
            printf("proc 'proc_wait_connection' finished\n");
            pthread_exit((void*)1);
        }

    }
    printf("proc 'proc_wait_connection' finished\n");
    pthread_exit((void*)1);
}


int main()
{
    printf("programm started\n");
    signal(SIGINT, sig_handler);
    signal(SIGPIPE, sig_handler);

    if (mkfifo(PIPE_PATH, FILE_MODE) == -1) perror("mkfifo");

    printf("thread 'proc_wait_connection' created with status %d\n", pthread_create(&thread1, NULL, proc_wait_connection, NULL));
    printf("waiting press button...\n");
    getchar();
    printf("button pressed\n");

    flag = 1;
    int *exitcode;
    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode1 = %p\n", exitcode);
    pthread_join(thread2, (void**)&exitcode);
    printf("exitcode2 = %p\n", exitcode);

    close(fd);
    unlink(PIPE_PATH);
    printf("programm finished\n");
    return 0;
}
