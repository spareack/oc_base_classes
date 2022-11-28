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
    struct timespec ts;
    clock_gettime (CLOCK_BOOTTIME, &ts);
    printf("child boot_time: %ld\n", ts.tv_sec);

    // sleep(2);

    printf("child process: parent_id = %d, my_id = %d\n", getppid(), getpid());
    printf("im child procces, my exit status %d\n", 0);
    exit(0);
}
