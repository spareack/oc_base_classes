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
#define SERVER_PATH "/dev/shm/server_sock.soc"
#define CLIENT_PATH "/dev/shm/client_sock.soc"


int receive_flag = 0;
int send_flag = 0;

int main_socket = 0;

struct sockaddr_un client_addr;
struct sockaddr_un server_addr;


void sig_handler(int count) 
{
    close(main_socket);
    unlink(CLIENT_PATH);
    printf("lost connection..\n");
    exit(1);
}

char *getSpecialValue()
{
    uid_t uid = getuid();
    struct passwd *pwuid = getpwuid(uid);
    return pwuid->pw_gecos;
}

void *send_thread(void *arg) {

    char data[DATA_SIZE] = "kak tebya zovut?";

    while(send_flag != 1)
    {
        if (sendto(main_socket, data, DATA_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) == -1) 
        {
            printf("there is no connection to server..\n");
            // perror("sendto");
        }
        else
        {
            printf("send data: %s\n", data);
        }
        sleep(1);
    }
    printf("proc 'send_thread' finished\n");
    pthread_exit((void*)1);
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
            // perror("recvfrom");
            sleep(1);
        } 
        else if (reccount == 0)
        {
            printf("waiting message\n");
            sleep(1);
        }
        else printf("received data: %s\n", data);
    }
    printf("proc 'receive_thread' finished\n");
    pthread_exit((void*)1);
}


int main()
{
    printf("programm started\n");
    signal(SIGINT, sig_handler);

    pthread_t thread1, thread2;

    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, CLIENT_PATH);

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SERVER_PATH);

    main_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    fcntl(main_socket, F_SETFL, O_NONBLOCK);

    unlink(CLIENT_PATH);
    bind(main_socket, (struct sockaddr*)&client_addr, sizeof(struct sockaddr_un));

    printf("thread 'receive_thread' created with status %d\n", pthread_create(&thread1, NULL, receive_thread, NULL));
    printf("thread 'send_thread' created with status %d\n", pthread_create(&thread2, NULL, send_thread, NULL));
    printf("waiting press button...\n");
    getchar();
    printf("button pressed\n");

    receive_flag = 1;
    send_flag = 1;

    int *exitcode;
    pthread_join(thread1, (void**)&exitcode);
    printf("exitcode1 = %p\n", exitcode);
    pthread_join(thread2, (void**)&exitcode);
    printf("exitcode2 = %p\n", exitcode);

    unlink(CLIENT_PATH);
    close(main_socket);
    printf("programm finished\n");
    return 0;
}
