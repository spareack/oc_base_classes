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
        memset(data, 0, DATA_SIZE);
        if (read(fd, data, DATA_SIZE) == 0)
        {
            printf("waiting for writer\n");
        }
        else
        {
            printf("received data: %s\n", data);
        }
        sleep(1);
    }
    printf("proc 'proc_main_cycle' finished\n");
    pthread_exit((void*)2);
}

int main()
{
    printf("programm started\n");
    signal(SIGINT, sig_handler);
    signal(SIGPIPE, sig_handler);

    if (mkfifo(PIPE_PATH, FILE_MODE) == -1) perror("mkfifo");
    fd = open(PIPE_PATH, O_RDONLY | O_NONBLOCK);


    printf("thread 'proc_wait_connection' created with status %d\n", pthread_create(&thread1, NULL, proc_main_cycle, NULL));
    printf("waiting press button...\n");
    getchar();
    printf("button pressed\n");

    flag = 1;
    int *exitcode;
    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode1 = %p\n", exitcode);

    close(fd);
    unlink(PIPE_PATH);
    printf("programm finished\n");
    return 0;
}
