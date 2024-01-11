#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <execinfo.h>

int shared1 = 0, shared2 = 0;
pthread_mutex_t mtx1 = PTHREAD_MUTEX_INITIALIZER, mtx2 = PTHREAD_MUTEX_INITIALIZER;
int interval = 0;


void quit(int sig){
	printf("\nHave a nice day :)\n");
    pthread_mutex_destroy(&mtx1);
    pthread_mutex_destroy(&mtx2);
	exit(EXIT_SUCCESS);
}

void *func1(void *arg) {
    int thread_id = *((int *)arg);

    while (1) {
        pthread_mutex_lock(&mtx1);
        pthread_mutex_lock(&mtx2);

        shared1++;
		shared2++;
        printf("Thread %d: using shared data %d and %d\n", thread_id, shared1, shared2);
        pthread_mutex_unlock(&mtx1);
        pthread_mutex_unlock(&mtx2);

        sleep(2);
    }
}

void *func2(void *arg) {
    int thread_id = *((int *)arg);

    while (1) {
        pthread_mutex_lock(&mtx2);
        pthread_mutex_lock(&mtx1);

        shared1++;
		shared2++;
        printf("Thread %d: using shared data %d and %d\n", thread_id, shared1, shared2);
        pthread_mutex_unlock(&mtx2);
        pthread_mutex_unlock(&mtx1);

        sleep(2);
    }
}

int main() {
    pthread_t thread1, thread2;
	signal(SIGINT, quit);
    int id1 = 1, id2 = 2;
	int flag = 0;

	char ans;
	printf("wanna make deadlock? (y/n)\n");
	scanf("%c", &ans);
	flag = (ans == 'y')? 1 : 0;

/*	
	void* arr[3];
	size_t n = backtrace(arr, 3);
	printf("%p\n", arr[0]);
	exit(0);
*/

	if(flag){
		if (pthread_create(&thread1, NULL, func1, (void *)&id1) != 0 || pthread_create(&thread2, NULL, func2, (void *)&id2) != 0) {
        	perror("Thread creation failed");
        	exit(EXIT_FAILURE);
    	}
	}else{
		if (pthread_create(&thread1, NULL, func1, (void *)&id1) != 0 || pthread_create(&thread2, NULL, func1, (void *)&id2) != 0) {
        	perror("Thread creation failed");
        	exit(EXIT_FAILURE);
    	}
	}
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&mtx1);
    pthread_mutex_destroy(&mtx2);

    return 0;
}

