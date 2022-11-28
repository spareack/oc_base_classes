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
#include <sys/msg.h>
#include <sys/ipc.h>


#define DATA_SIZE 256
int flag = 0;
int msgid;

typedef struct {
    long mtype;
    char buff[DATA_SIZE];
} TMessage ;
TMessage message;


char *getSpecialValue()
{
    uid_t uid = getuid();
    struct passwd *pwuid = getpwuid(uid);
    return pwuid->pw_gecos;
}

void intHandler(int count) 
{
    msgctl(msgid, IPC_RMID, NULL);
    exit(EXIT_FAILURE);
}

void *proc_main_cycle(void *arg)
{
    char data[DATA_SIZE];
    while(flag != 1) 
    {
        memset(message.buff, 0, DATA_SIZE);

        int result = msgrcv(msgid, &message, DATA_SIZE, message.mtype, IPC_NOWAIT);
        if (result == -1) perror("msgrcv");
        else
        {
            strncpy(data, message.buff, DATA_SIZE);
            printf("receive data: %s\n", data);
        }
        sleep(1);
    }
    printf("proc 'proc_main_cycle' finished\n");
    pthread_exit((void*)1);
}


int main()
{
    printf("programm started\n");
    signal(SIGINT, intHandler);

    pthread_t thread1;
    message.mtype = 1;

    int key = ftok("lab1", 'A');
    msgid = msgget(key, 0);
    if (msgid < 0) msgid = msgget(key, IPC_CREAT);


    printf("thread created with status %d\n", pthread_create(&thread1, NULL, proc_main_cycle, NULL));
    printf("waiting press button...\n");
    getchar();
    printf("button pressed\n");

    flag = 1;
    int *exitcode;
    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode1 = %p\n", exitcode);

    msgctl(msgid, IPC_RMID, NULL);

    printf("programm finished\n");
    return 0;
}
