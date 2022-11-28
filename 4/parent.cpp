#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}


int main(int argc, char **argv, char **envp)
{
    printf("programm started \n");
    pid_t pid = fork();
    
    if (pid == 0)
    {
        execle("child", "first arg", "second arg", NULL, envp);
    }
    else if (pid > 0)
    {
        printf("parent process: parent_id = %d, my_id = %d, child_id = %d\n", getppid(), getpid(), pid);

        int status;
        int check = waitpid(pid, &status, WNOHANG);
        while (check == 0)
        {
            printf("waiting...\n");
            msleep(500);
            check = waitpid(pid, &status, WNOHANG);
        }

        if (check == -1)
        {
            perror("waitpid");
            return -1;
        }
        else
        {
            if (status != 0) printf("error in child process\n");
            else printf("child procces finished with status %d\n", status);
            return status;
        }
    }
    else perror("fork");
    return 0;
}
