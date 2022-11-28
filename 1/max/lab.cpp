#include <pthread.h>
#include <unistd.h>
#include <cstdio>
// using namespace std;
// #include <iostream>

typedef struct
{
    int flag;
    char sym;
} targs;


static void* thread1_start(void* arg1)
{
    targs* args = (targs*) arg1;
    while (args->flag == 0)
    {
        putchar(args->sym);
        fflush(stdout);
        sleep(1);
    }
    pthread_exit((void*)1);
}

static void* thread2_start(void* arg2)
{
    targs* args = (targs*) arg2;
    while (args->flag == 0)
    {
        putchar(args->sym);
        fflush(stdout);
        sleep(1);
    }
    pthread_exit((void*)2);
}

int main()
{
    targs arg1, arg2;

    arg1.flag = 0;
    arg1.sym = '1';
    arg2.flag = 0;
    arg2.sym = '2';

    pthread_t id1;
    pthread_t id2;

    // thread1_start(&arg1);
    // thread2_start(&arg2);

    pthread_create(&id1, NULL, thread1_start, &arg1);
    pthread_create(&id2, NULL, thread2_start, &arg2);

    getchar();

    arg1.flag = 1;
    arg2.flag = 1;

    int* exitcode;

    pthread_join(id1, (void**)&exitcode);
    cout << "exit code " << exitcode << endl;
    pthread_join(id2, (void**)&exitcode);
    cout << "exit code " << exitcode << endl;

    return 0;
}