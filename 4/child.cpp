#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char **argv, char **envp)
{
    printf("child process: get %d args!\n", argc);
    for(int i = 0; i < argc; i += 1)
    {
        printf("%s\n", argv[i]);
        sleep(1);
    }

    printf("print some envp values...\n");
    int count = 0;
    // while(count < 3)
    while(envp[count] != NULL)
    {
        printf("%s\n", envp[count]);
        count += 1;
        sleep(1);
    }

    printf("child process: parent_id = %d, my_id = %d\n", getppid(), getpid());
    printf("im child procces, my exit status %d\n", 0);
    exit(0);
}
