#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

typedef struct
{
	int flag;
}flags;


void *proc1(void *arg)
{
	flags *args = (flags *) arg;
	printf("\nThread 1 has started\n");
	while (args->flag == 0)
	{
		putchar('1');
		fflush(stdout);
		sleep(1);
	}
	pthread_exit((void *) 1);
	printf("\nThread 1 finished\n");
}

void *proc2(void *arg)
{
	flags *args = (flags *) arg;
	printf("\nThread 2 has started\n");
	while (args->flag == 0)
	{
		putchar('2');
		fflush(stdout);
		sleep(1);
	}
	pthread_exit((void *) 2);
	printf("\nThread 2 finished\n");
}

int main()
{
	struct timespec ts_main;

	if (clock_gettime(CLOCK_REALTIME, &ts_main) != 0) perror("clock_gettime");

	int *exitcode1, *exitcode2;
	flags arg1;
	flags arg2;
	arg1.flag = 0;
	arg2.flag = 0;
	pthread_t id1;
	pthread_t id2;
	printf("\nThe program has started\n");
	pthread_create(&id1, NULL, proc1, &arg1);
	pthread_create(&id2, NULL, proc2, &arg2);
	printf("\nThe program is waiting for a keystroke\n");
	getchar();
	printf("\nKey pressed\n");

	for (size_t i = 0; i < 1000000; i++) getppid();

	struct timespec ts;
	struct timespec ts_res;
	clockid_t cid;


	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts) != 0) perror("clock_gettime");
	clock_getres(CLOCK_PROCESS_CPUTIME_ID, &ts_res);
	printf("main thread: %5jd.%04ld (with res = %2jd.%09ld)\n", (intmax_t) ts.tv_sec, ts.tv_nsec / 100000, 
																		(intmax_t) ts_res.tv_sec, ts_res.tv_nsec);

	pthread_getcpuclockid(id1, &cid);
	if (clock_gettime(cid, &ts) != 0) perror("clock_gettime");
	clock_getres(cid, &ts_res);
	printf("thread1 (id1): %3jd.%04ld (with res = %2jd.%09ld)\n", (intmax_t) ts.tv_sec, ts.tv_nsec / 100000,
																	(intmax_t) ts_res.tv_sec, ts_res.tv_nsec);

	pthread_getcpuclockid(id2, &cid);
	if (clock_gettime(cid, &ts) != 0) perror("clock_gettime");
	clock_getres(cid, &ts_res);
	printf("thread2 (id1): %3jd.%04ld (with res = %2jd.%09ld)\n", (intmax_t) ts.tv_sec, ts.tv_nsec / 100000,
																	(intmax_t) ts_res.tv_sec, ts_res.tv_nsec);

	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) != 0) perror("clock_gettime");
	clock_getres(CLOCK_THREAD_CPUTIME_ID, &ts_res);
	printf("whole proccess: %2jd.%04ld (with res = %2jd.%09ld)\n", (intmax_t) ts.tv_sec, ts.tv_nsec / 100000,
																	(intmax_t) ts_res.tv_sec, ts_res.tv_nsec);

	struct timespec ts_main_temp;
	if (clock_gettime(CLOCK_REALTIME, &ts_main_temp) != 0) perror("clock_gettime");

	ts_main_temp.tv_sec -= ts_main.tv_sec;
	ts_main_temp.tv_nsec -= ts_main.tv_nsec;
	if (ts_main_temp.tv_nsec < 0)
	{
		ts_main_temp.tv_sec -= 1;
		ts_main_temp.tv_nsec*= -1;
	}
	clock_getres(CLOCK_REALTIME, &ts_res);
	printf("main realtime: %3jd.%04ld (with res = %2jd.%09ld)\n", (intmax_t) ts_main_temp.tv_sec, ts_main_temp.tv_nsec / 100000,
																	(intmax_t) ts_res.tv_sec, ts_res.tv_nsec);

	arg1.flag = 1;
	arg2.flag = 1;
	pthread_join(id1, (void **) &exitcode1);
	pthread_join(id2, (void **) &exitcode2);
	printf("\nexitcode = %p\n", exitcode1);
	printf("exitcode = %p\n", exitcode2);
	printf("\nThe program finished\n");
}
