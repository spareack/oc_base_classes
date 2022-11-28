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

const unsigned short STR_LEN = 30;
const unsigned short MSG_LIST_SIZE = 10;
int unshift(char** arr, char* result, size_t len);
int pushBack(char** arr, char* str, size_t len);
int myGetChar();

struct targs
{
    int flag;
    int sock;
    pthread_t recieve;
    pthread_t send;
    struct targs* recieveArgs;
    struct targs* sendArgs;
};

void* sendRequest(void* arg) {
    printf("sendRequest started!\n");
    targs *args = (targs*) arg;
    int res;
    while(args->flag){
        char request[30] = "give me domain name";
        res = send(args->sock, request, STR_LEN, 0);
        if(res == -1){
            perror("send");
        } else {
            printf("SEND: %s\n", request);
        }
        sleep(1);
    }
    printf("sendRequest finished!");
    pthread_exit((void*)0);
}

void* recieveResponse(void* arg){
    printf("recieve response started!\n");
    targs *args = (targs*) arg;
    int reccount;
    char BUFFER[30];
    while(args->flag){
        reccount = recv(args->sock, BUFFER, STR_LEN, 0);
        if(reccount == -1){
            perror("recv");
        } else if(reccount == 0){
            shutdown(args->sock, 2);
            close(args->sock);
        } else {
            printf("RECIEVED: %s\n", BUFFER);
        }
        sleep(1);
    }
    printf("recieve response finished!\n");
    pthread_exit((void*)0);
}

void* connector(void* arg){
    printf("connectior started!\n");
    targs *args = (targs*) arg;
    struct sockaddr_in serverSocketConf;
    serverSocketConf.sin_family = AF_INET;
    serverSocketConf.sin_port = htons(3000);
    serverSocketConf.sin_addr.s_addr = inet_addr("127.0.0.1");
    while(args->flag){
        int res = connect(args->sock, (sockaddr*)&serverSocketConf, sizeof(serverSocketConf));
        if(res != 0){
            printf("connecting...\n");
            sleep(1);
        }
        else {
            printf("connected!\n");
            pthread_create(&args->send, NULL, sendRequest, args->sendArgs);
            pthread_create(&args->recieve, NULL, recieveResponse, args->recieveArgs);
            args->flag = 0;
        }
    }
    printf("connectior finished!\n");
    pthread_exit((void*)0);
}

void sighandler(int SIGNAL){
    printf("disconnected\n");
    exit(0);
}

int main() {
    printf("Client programm started!\n");
    signal(SIGPIPE, sighandler);

    pthread_t con, recieve, send; // создать идентификаторы потоков
    targs argCon, argRec, argSend;
    argCon.flag = 1;
    argCon.recieve = recieve; argCon.send = send;
    argCon.recieveArgs = &argRec;
    argCon.sendArgs = &argSend;
    argRec.flag = 1; 
    argSend.flag = 1;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); //создать сокет для работы с сервером
    if(serverSocket == -1){
        perror("server socket");
        return 1;
    }
    argCon.sock = serverSocket;
    argRec.sock = serverSocket;
    argSend.sock = serverSocket;
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);
    pthread_create(&con, NULL, connector, &argCon);
    myGetChar();
    argCon.flag = 0;
    argRec.flag = 0;
    argSend.flag = 0;
    int* exitcodes[3];
    pthread_join(con, (void**)&exitcodes[0]);
    pthread_join(recieve, (void**)&exitcodes[1]);
    pthread_join(send, (void**)&exitcodes[2]);
    int res = shutdown(serverSocket, SHUT_RDWR);
    if(res != 0){
        perror("shutdown");
        return 2;
    }
    res = close(serverSocket);
    if(res != 0){
        perror("close");
        return 3;
    }
    printf("Client programm finished!\n");
    return 0;
}

int myGetChar(){
    printf("Press the button!\n");
    int result = getchar();
    printf("Button pressed!\n");
    return result;
}
