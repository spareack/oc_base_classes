#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <cerrno>


void* proc1(void*)
{
    printf("proc1 started\n");
    sigset_t set;
    sigemptyset(&set);
    
    if (sigaddset(&set, SIGUSR1) != 0) 
    {                                           
        perror("Sigaddset error");
        pthread_exit((void *)1);
    }
    int check;
    if (sigwait(&set, &check) != 0 || check != SIGUSR1)
    {
        perror("Sigwait error");
        pthread_exit((void *)2);
    }
    pthread_exit((void *)0); 
}

void* proc2(void*)
{
    printf("proc2 started\n");
    sigset_t set;
    sigemptyset(&set);

    if (sigaddset(&set, SIGUSR1) != 0) 
    {
        perror("Sigaddset error");
        pthread_exit((void *)1);
    }
    int check;
    if (sigwait(&set, &check) != 0 || check != SIGUSR1)
    {
        perror("Sigwait error");
        pthread_exit((void *)2);
    }
    pthread_exit((void *)0); 
}

int main()
{
    printf("programm started\n");
    pthread_t thread1, thread2;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    pthread_create(&thread1, NULL, proc1, NULL);
    pthread_create(&thread2, NULL, proc2, NULL);

    printf("waiting press button...\n");
    getchar();
    printf("button pressed \n");

    if (pthread_kill(thread1, SIGUSR1) + pthread_kill(thread2, SIGUSR1) > 0)
    {
        perror("pthread_kill error");
        return 1;
    }
    else printf("processes killed \n");
    
    int *exitcode = (int*)-1;
    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);
    pthread_join(thread2, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);

    printf("programm finished \n");
    return 0;
}
