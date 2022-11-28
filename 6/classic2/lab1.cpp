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

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define SEM_READ_NAME "/spec_semRead"
#define SEM_WRITE_NAME "/spec_semWrite"
#define ADDR_NAME "/spec_addr"


int flag = 0;
sem_t *semRead;
sem_t *semWrite;
int fd;

#define ADDR_SIZE 25
char *addr;


void intHandler(int count) 
{
    sem_close(semRead);
    sem_unlink(SEM_READ_NAME);
    sem_close(semWrite);
    sem_unlink(SEM_WRITE_NAME);

    munmap(addr, ADDR_SIZE);
    close(fd);
    shm_unlink(ADDR_NAME);
    exit(EXIT_FAILURE);
}

char *getSpecialValue()
{
    uid_t uid = getuid();
    struct passwd *pwuid = getpwuid(uid);
    return pwuid->pw_gecos;
}

void* proc(void *arg)
{
    printf("proc started\n");
    while(flag != 1)
    {
        if (sem_wait(semRead) == -1) perror("sem_wait");

        char *value = getSpecialValue();
        strncpy(addr, value, ADDR_SIZE);
        printf("send data: %s\n", addr);

		if (sem_post(semWrite) == -1) perror("sem_post");
        sleep(1);
    }
    printf("proc finished\n");
    pthread_exit((void*)1);
}


int main()
{
    printf("programm started\n");
    signal(SIGINT, intHandler);

    pthread_t thread1;
    fd = shm_open(ADDR_NAME, O_CREAT | O_RDWR, FILE_MODE);
    if (fd == -1) perror("shm_open");
    if (ftruncate(fd, ADDR_SIZE) == -1) perror("ftruncate");
    
    addr = (char*)mmap(NULL, ADDR_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) return 2;

    semWrite = sem_open(SEM_WRITE_NAME, O_CREAT, FILE_MODE, 1);
    semRead = sem_open(SEM_READ_NAME, O_CREAT, FILE_MODE, 1);

    printf("thread created with status %d\n", pthread_create(&thread1, NULL, proc, NULL));

    printf("waiting press button...\n");
    getchar();
    printf("button pressed\n");

    flag = 1;
    int *exitcode;
    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);

    sem_close(semRead);
    sem_unlink(SEM_READ_NAME);
    sem_close(semWrite);
    sem_unlink(SEM_WRITE_NAME);

    munmap(addr, ADDR_SIZE);
    close(fd);
    shm_unlink(ADDR_NAME);
    printf("programm finished\n");
    return 0;
}
