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
#include <vector>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>

using namespace std;

#define DATA_SIZE 256
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define sem_name "/sem_inst"
#define SERVER_PATH "/dev/shm/server_sock.soc"
#define CLIENT_PATH "/dev/shm/client_sock.soc"

sem_t *sem_inst;

int receive_flag = 0;
int handle_response_flag = 0;

int main_socket = 0;
vector<string> msglist;

struct sockaddr_un client_addr;
struct sockaddr_un server_addr;


void sig_handler(int count) 
{
    close(main_socket);
    sem_close(sem_inst);
    sem_unlink(sem_name);
    unlink(SERVER_PATH);

    printf("lost connection..\n");
    exit(1);
}

char *getSpecialValue()
{
    uid_t uid = getuid();
    struct passwd *pwuid = getpwuid(uid);
    return pwuid->pw_gecos;
}


void *receive_thread(void *arg) 
{
    char data[DATA_SIZE];
    unsigned int addr_len = sizeof(struct sockaddr_un);

    while(receive_flag != 1)
    {
        memset(data, 0, DATA_SIZE);

        int reccount = recvfrom(main_socket, data, DATA_SIZE, 0, NULL, NULL);
        if (reccount == -1) 
        {
            printf("waiting client..\n");
            // perror("recvfrom");
            sleep(1);
        }
        else if (reccount == 0)
        {
            printf("waiting message\n");
            sleep(1);
        }
        else 
        {
            if (sem_wait(sem_inst) == -1) perror("sem_wait");
            msglist.push_back(string(data));
            if (sem_post(sem_inst) == -1) perror("sem_post");
            printf("received data: %s\n", data);
        }
    }
    printf("proc 'receive_thread' finished\n");
    pthread_exit((void*)1);
}

void *handle_response_thread(void *arg) {
    while(handle_response_flag != 1)
    {
        if (sem_wait(sem_inst) == -1) perror("sem_wait");

        if (!msglist.empty()) 
        {
            string S = msglist.back();
            msglist.pop_back();
            if (sem_post(sem_inst) == -1) perror("sem_post");

            char* data = getSpecialValue();

            if (sendto(main_socket, data, DATA_SIZE, 0, (struct sockaddr*)&client_addr, sizeof(struct sockaddr_un)) == -1) perror("sendto");
            else
            {
                printf("send data: %s\n", data);
            }
        }
        else
        {
            if (sem_post(sem_inst) == -1) perror("sem_post");
            sleep(1);
        }

    }
    printf("proc 'handle_response_thread' finished\n");
    pthread_exit((void*)1);
}


int main()
{
    printf("programm started\n");
    signal(SIGINT, sig_handler);

    pthread_t thread1, thread2;
    sem_inst = sem_open(sem_name, O_CREAT, FILE_MODE, 1);

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SERVER_PATH);

    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, CLIENT_PATH);

    main_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (main_socket == -1) perror("socket");
    fcntl(main_socket, F_SETFL, O_NONBLOCK);

    unlink(SERVER_PATH);
    if (bind(main_socket, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) == -1) perror("bind");


    printf("thread 'receive_thread' created with status %d\n", pthread_create(&thread1, NULL, receive_thread, NULL));
    printf("thread 'handle_response_thread' created with status %d\n", pthread_create(&thread2, NULL, handle_response_thread, NULL));
    printf("waiting press button...\n");
    getchar();
    printf("button pressed\n");

    receive_flag = 1;
    handle_response_flag = 1;

    int *exitcode;
    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode1 = %p\n", exitcode);
    pthread_join(thread2, (void**)&exitcode);
    printf("exitcode2 = %p\n", exitcode);

    sem_close(sem_inst);
    sem_unlink(sem_name);
    unlink(SERVER_PATH);
    close(main_socket);
    printf("programm finished\n");
    return 0;
}
