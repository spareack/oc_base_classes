#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>
#include <time.h>
#include<sstream>
#include<string>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sched.h>
using namespace std;
#define CLONE_NEWTIME 0x00000080
#define OFFSET (36000)

int get_file(string path)
{
    string line;
    ifstream in(path);
    if (in.is_open())
    {
        while (getline(in, line))
        {
            cout << line << endl;
        }
    }
    in.close();
    return 0;
}

int write_file(string path, string value)
{
    ofstream out;
    out.open(path, fstream::out );
    perror("open");
    if (out.is_open())
    {
        out << "asdsaddsasad" << endl;
    }
    return 0;
}

int old_write_file(string path, string value)
{
    FILE * pf;
    // char buffer [100] = ;

    pf = fopen(path.c_str() , "w+");
    perror("fopen");
    if (pf == NULL) perror("Error opening file");
    else
    {
        fputs(value.c_str(), pf);
        perror("fputs");
        fclose(pf);
    }
    return 0;
}

int old_old_write_file(string path, string value)
{
    int fd = open(path.c_str(), O_CREAT | O_RDWR, S_IRUSR);
    perror("open");
    write(fd, value.c_str(), 100);
    perror("write");
    close(fd);
    return 0;
}

int set_boottime_offset(int child_pid, long int tv_sec, long int tv_nsec)
{
    stringstream ss;
    string tv_nsec_str, tv_sec_str, child_pid_str;
    ss << child_pid;
    ss >> child_pid_str;
    ss << tv_sec;
    ss >> tv_sec_str;
    ss << tv_nsec;
    ss >> tv_nsec_str;

    string new_value = "monotonic 0 0%nboottime " + tv_sec_str + " " + tv_nsec_str;
    string name = "/proc/" + child_pid_str + "/timens_offsets";
    // string name = "/proc/self/timens_offsets";

    // string bash_comm = "echo \"monotonic 10000 0\" > " + name;
    // system(bash_comm.c_str());


    // get_file(name);
    old_old_write_file(name, new_value);
    // old_write_file(name, new_value);
    // write_file(name, new_value);
    get_file(name);

    return 0;
}

static inline int _settime(clockid_t clk_id, time_t offset)
{
	int fd, len;
	char buf[4096];

	len = snprintf(buf, sizeof(buf), "%d %ld 0", clk_id, offset);
	fd = open("/proc/self/timens_offsets", O_WRONLY);

    write(fd, buf, len);

	close(fd);
	return 0;
}

int main(int argc, char **argv, char **envp)
{
    printf("programm started \n");

    struct timespec ts;
    clock_gettime (CLOCK_BOOTTIME, &ts);
    printf("parent boot_time: %ld\n", ts.tv_sec);

    unshare(CLONE_NEWTIME);
    if (_settime(CLOCK_BOOTTIME, OFFSET)) return 1;
    
    pid_t pid = fork();


    if (pid == 0)
    {
        // sleep(1);
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
            // msleep(500);
            sleep(1);
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
