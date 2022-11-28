#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <cstring>
#include <pwd.h>

typedef struct
{
    int flag;
    char sym;
    int *filedes;
} targs;


char *getSpecialValue()
{
    uid_t uid = getuid();
    struct passwd *pwuid = getpwuid(uid);
    return pwuid->pw_gecos;
}

void* proc1(void *arg)
{
    printf("proc1 started\n");
    targs *args = (targs*) arg;
    char value[255];

    while(args->flag != 1)
    {
        char *value = getSpecialValue();
        if (write(args->filedes[1], value , sizeof(value)) == -1) perror("write");
        else printf("send: %s\n", value);
        sleep(1);
    }
    
    printf("proc1 finished\n");
    pthread_exit((void *)1); 
}

void* proc2(void *arg)
{
    printf("proc2 started\n");
    targs *args = (targs*) arg;
    char value[255];

    while(args->flag != 1)
    {
        memset(value, 0, sizeof(value));
        if (read(args->filedes[0], value, sizeof(value)) == -1) perror("read");
        else printf("receive: %s\n", value);
    }

    printf("proc2 finished\n");
    pthread_exit((void *)2); 
}


int main()
{
    printf("programm started\n");
    pthread_t thread1, thread2;
    targs arg1, arg2;
    int filedes[2];
    if (pipe(filedes) != 0) perror("pipe");

    arg1.flag = 0; arg1.sym = '1'; arg1.filedes = filedes;
    arg2.flag = 0; arg2.sym = '2'; arg2.filedes = filedes;
    pthread_create(&thread1, NULL, proc1, &arg1);
    pthread_create(&thread2, NULL, proc2, &arg2);

    printf("waiting press button...\n");
    getchar();
    printf("button pressed \n");

    arg1.flag = 1; arg2.flag = 1;
    int *exitcode = (int*)-1;

    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);
    pthread_join(thread2, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);

    close(filedes[0]); close(filedes[1]);
    printf("programm finished \n");
    return 0;
}
