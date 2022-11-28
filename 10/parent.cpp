#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>
#include <ev.h>


static void child_exit_cb(EV_P_ ev_child *w, int revents)
{
    ev_child_stop(EV_A_ w);
    ev_break(EV_A_ EVBREAK_ALL);

    printf("parent: handle child process finish, exit\n");
    exit(0);
}


int main(int argc, char **argv, char **envp)
{
    printf("parent: programm started \n");

    ev_child child_watcher;
    struct ev_loop *loop = EV_DEFAULT;

    pid_t pid = fork();
    
    if (pid == 0)
    {
        execle("child", "first arg", "second arg", NULL, envp);
    }
    else if (pid > 0)
    {
        printf("parent: parent_id = %d, my_id = %d, child_id = %d\n", getppid(), getpid(), pid);
        printf("parent: start waiting child exit...\n");

        ev_child_init (&child_watcher, child_exit_cb, pid, 0);
        ev_child_start(EV_DEFAULT,  &child_watcher);

        ev_run(EV_DEFAULT_  0);
    }
    else perror("fork");
    return 0;
}
