#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <cstring>
#include <pwd.h>

#define BUFF_SIZE 255

typedef struct
{
    int flag;
    char sym;
    int *filedes;
} targs;


char* get_current_datetime(char *buffer)
{
    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    char* format = const_cast<char*>("%Y.%m.%d %I:%M:%S");
    strftime(buffer, BUFF_SIZE, format, timeinfo);
    return buffer;
}

void* proc1(void *arg)
{
    printf("\nThread 1 has started\n");
    targs *args = (targs*) arg;
    char value[BUFF_SIZE];

    while(args->flag != 1)
    {
        get_current_datetime(value);
        if (write(args->filedes[1], value , sizeof(value)) == -1) perror("write");
        else printf("send: %s\n", value);
        sleep(1);
    }
    
    printf("\nThread 1 finished\n");
    pthread_exit((void *)1); 
}

void* proc2(void *arg)
{
    printf("\nThread 2 has started\n");
    targs *args = (targs*) arg;
    char value[BUFF_SIZE];

    while(args->flag != 1)
    {
        memset(value, 0, sizeof(value));
        if (read(args->filedes[0], value, sizeof(value)) == -1) perror("read");
        else printf("receive: %s\n", value);
    }

    printf("\nThread 2 finished\n");
    pthread_exit((void *)2); 
}


int main()
{
    printf("\nThe program has started\n");
    pthread_t thread1, thread2;
    targs arg1, arg2;
    int filedes[2];
    if (pipe(filedes) != 0) perror("pipe");

    arg1.flag = 0; arg1.sym = '1'; arg1.filedes = filedes;
    arg2.flag = 0; arg2.sym = '2'; arg2.filedes = filedes;
    pthread_create(&thread1, NULL, proc1, &arg1);
    pthread_create(&thread2, NULL, proc2, &arg2);

    printf("\nThe program is waiting for a keystroke\n");
    getchar();
    printf("\nKey pressed\n");

    arg1.flag = 1; arg2.flag = 1;
    int *exitcode = (int*)-1;

    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);
    pthread_join(thread2, (void**)&exitcode);
    printf("exitcode = %p\n", exitcode);

    close(filedes[0]); close(filedes[1]);
    printf("\nThe program finished\n");
    return 0;
}
