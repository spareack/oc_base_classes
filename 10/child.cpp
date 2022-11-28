#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ev.h>


int main(int argc, char **argv, char **envp)
{
    printf("child: process started \n");

    for(int i = 0; i < argc; i += 1)
    {
        printf("child: %s\n", argv[i]);
        sleep(1);
    }

    printf("child: im done, exit\n");
    exit(0);
}
