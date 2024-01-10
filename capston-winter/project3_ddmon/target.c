#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

int shared1 = 0, shared2 = 0;
pthread_mutex_t mtx1 = PTHREAD_MUTEX_INITIALIZER, mtx2 = PTHREAD_MUTEX_INITIALIZER;
int interval = 0;


void quit(int sig){
	printf("\nHave a nice day :)\n");
	exit(EXIT_SUCCESS);
}

void *func1(void *arg) {
    int thread_id = *((int *)arg);

    while (1) {
		sleep(1);
        pthread_mutex_lock(&mtx1);
		sleep(interval);
        pthread_mutex_lock(&mtx2);

        shared1++;
		shared2++;
        printf("Thread %d: using shared data %d and %d\n", thread_id, shared1, shared2);
        pthread_mutex_unlock(&mtx1);
		sleep(interval);
        pthread_mutex_unlock(&mtx2);

        sleep(1);
    }
}

void *func2(void *arg) {
    int thread_id = *((int *)arg);

    while (1) {
        pthread_mutex_lock(&mtx2);
		sleep(interval);
        pthread_mutex_lock(&mtx1);

        shared1++;
		shared2++;
        printf("Thread %d: using shared data %d and %d\n", thread_id, shared1, shared2);
        pthread_mutex_unlock(&mtx2);
		sleep(interval);
        pthread_mutex_unlock(&mtx1);

        sleep(1);
    }
}

int main() {
    pthread_t thread1, thread2;
	signal(SIGINT, quit);
    int id1 = 1, id2 = 2;

	char ans;
	printf("wanna make deadlock? (y/n)\n");
	scanf("%c", &ans);
	interval = (ans == 'y')? 10 : 0;	

    if (pthread_create(&thread1, NULL, func1, (void *)&id1) != 0 ||
        pthread_create(&thread2, NULL, func2, (void *)&id2) != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&mtx1);
    pthread_mutex_destroy(&mtx2);

    return 0;
}

