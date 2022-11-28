#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

char* msglist[10];
const unsigned short STR_LEN = 30;
const unsigned short MSG_LIST_SIZE = 10;
int acceptSock;
int unshift(char** arr, char* result, size_t len);
int pushBack(char** arr, char* str, size_t len);
int myGetChar();
sem_t semaphore;

struct targs
{
    int flag;
    int sock;
    pthread_t recieve;
    pthread_t send;
    struct targs* recieveArgs;
    struct targs* sendArgs;
};

void* recieveRequest(void* arg) {
    printf("reciever started!\n");
    targs *args = (targs*) arg;
    int reccount;
    char BUFFER[30];
    while(args->flag){
        reccount = recv(acceptSock, BUFFER, STR_LEN, 0);
        if(reccount == -1){
            perror("recv");
            sleep(1);
        } else if(reccount == 0){
            shutdown(acceptSock, 2);
            close(acceptSock);
        } else {
            sem_wait(&semaphore);
            pushBack(msglist, BUFFER, STR_LEN);
            sem_post(&semaphore);
            printf("RECIEVED: %s\n", BUFFER);
            sleep(1);
        }
    }
    printf("reciever finished!\n");
    pthread_exit((void*)0);
}

void* responseForm(void* arg){
    printf("responser started!\n");
    targs *args = (targs*) arg;
    char request[30];
    char BUFFER[30];
    int sendcount;
    while(args->flag){
        sem_wait(&semaphore);
        unshift(msglist, request, MSG_LIST_SIZE);
        if(request){

            if (getdomainname(BUFFER, sizeof(BUFFER)) == -1) printf("Error in creating proc1");

            sendcount = send(acceptSock, BUFFER, STR_LEN, 0);
            if(sendcount == -1){
                perror("sendcount");
                // sleep(1);
            } else {
                printf("SEND: %s ||", BUFFER);
            }
        } 
        sem_post(&semaphore);
        sleep(1);
    }
    printf("responser finished!\n");
    pthread_exit((void*)0);
}

void* connectWaiter(void* arg){
    printf("connection waiting has started!\n");
    targs *args = (targs*) arg;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3000);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int addrlen = sizeof(addr);
    while(args->flag){
        acceptSock = accept(args->sock, NULL, NULL);
        if(acceptSock == -1){
            perror("accept");
            sleep(1);
        } else {
            pthread_create(&args->recieve, NULL, recieveRequest, args->recieveArgs);
            pthread_create(&args->send, NULL, responseForm, args->sendArgs);
            args->flag = 0;
        }
    }
    printf("connection waiting has finished!\n");
    pthread_exit((void*)0);
}

void sighandler(int SIGNAL){
    printf("disconnected\n");
    exit(0);
}

int main() {
    printf("Server programm started!\n");
    for(int i = 0; i < MSG_LIST_SIZE; i++){
        msglist[i] = (char*) malloc(sizeof(char) * STR_LEN);
    }
    signal(SIGPIPE, sighandler);
    char BUFFER[30];
    pthread_t resp, recieve, wait;
    targs argWaitConnect, argRecieve, argSendResponse;
    argWaitConnect.flag = 1;
    argWaitConnect.recieve = recieve; 
    argWaitConnect.send = resp;
    argWaitConnect.recieveArgs = &argRecieve;
    argWaitConnect.sendArgs = &argSendResponse;
    argRecieve.flag = 1; 
    argSendResponse.flag = 1;
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(listenSocket == -1){
        perror("listen socket");
        return 1;
    }
    fcntl(listenSocket, F_SETFL, O_NONBLOCK);
    argWaitConnect.sock = listenSocket; 
    argRecieve.sock = listenSocket; 
    argSendResponse.sock = listenSocket;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3000);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int res = bind(listenSocket, (sockaddr*)&addr, sizeof(addr));
    if(res == -1){
        perror("bind");
        return 2;
    }
    int optval = 1;
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    res = listen(listenSocket, 10);
    if(res == -1){
        perror("listen");
        return 3;
    }
    sem_init(&semaphore, 0, 1);
    pthread_create(&wait, NULL, connectWaiter, &argWaitConnect);
    myGetChar();
    argRecieve.flag = 0; 
    argSendResponse.flag = 0; 
    argWaitConnect.flag = 0;
    int* exitcodes[3];
    pthread_join(wait, (void**)&exitcodes[0]);
    pthread_join(recieve, (void**)&exitcodes[1]);
    pthread_join(resp, (void**)&exitcodes[2]);
    shutdown(listenSocket, 2);
    shutdown(acceptSock, 2);
    close(listenSocket);
    close(acceptSock);
    printf("Server programm finished!\n");
    for(int i = 0; i < MSG_LIST_SIZE; i++){
        free(msglist[i]);
    }
    return 0;
}


int myGetChar(){
    printf("Press the button!\n");
    int result = getchar();
    printf("Button pressed!\n");
    return result;
}

int pushBack(char** arr, char* str, size_t len) 
{
    for(int i = 0; i < len; i++) 
    {
        if (*arr[i] != '\0') continue;
        else 
        {
            strcpy(arr[i], str);
            return 0;
        }
    }
    return -1;
}

int unshift(char** arr, char* result, size_t len)
{
    strcpy(result, arr[0]);
    *arr[0] = '\0';

    if (*arr[len-1] == '\0') for(int i = 0; i < len-1; i++) strcpy(arr[i], arr[i+1]);
    else 
    {
        for(int i = 0; i < len-1; i++) strcpy(arr[i], arr[i+1]);
        *arr[len-1] = '\0';
    }
    return 0;
}
