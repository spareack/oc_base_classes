#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>
#include <string.h>

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

int add_curr_dir_to_path()
{
    char *PATH = getenv("PATH");
    char *currentDir = get_current_dir_name();
    strcat(PATH, ":");
    strcat(PATH, currentDir);
    return 0;
}

int main(int argc, char **argv, char **envp)
{
    printf("\nThe program has started\n");

    add_curr_dir_to_path();

    pid_t pid = fork();
    
    if (pid == 0)
    {
        char *new_argv[2];
        new_argv[0] = (char*)"first arg";
        new_argv[1] = (char*)"second arg";

        if (execvpe("child", new_argv, envp) == -1 ) perror("execvpe");
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
