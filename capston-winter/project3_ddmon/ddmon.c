#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define BUF_SIZE 128
#define BACKTRACE_SIZE 5
#include <pthread.h>

int makeFIFO(){
	if(mkfifo(".ddtrace", 0666)){
		if(errno != EEXIST) {
			perror("fail to open fifo: ");
			exit(EXIT_FAILURE);
		}
	}
	int fd = open(".ddtrace", O_WRONLY | O_SYNC);
	return fd;
}

int (*lock_cp)(pthread_mutex_t*) = NULL; 
int (*unlock_cp)(pthread_mutex_t*) = NULL;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
void init_real_one(){
	char * error ;
	lock_cp = dlsym(RTLD_NEXT, "pthread_mutex_lock") ;
	if ((error = dlerror()) != 0x0) exit(1) ;
	
	unlock_cp = dlsym(RTLD_NEXT, "pthread_mutex_unlock") ;
	if ((error = dlerror()) != 0x0) exit(1) ;

}


int pthread_mutex_lock(pthread_mutex_t* mutex){
	static __thread int n_lock = 0;
	n_lock += 1;

	if(lock_cp == NULL) init_real_one();
	/*j
	int (*lock_cp)(pthread_mutex_t* mutex);
	char* error;
	lock_cp = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if((error = dlerror()) != 0x0) exit(1);
	*/
	int rtn = lock_cp(mutex);

	if(n_lock == 1){
		lock_cp(&mtx);
		int i;
		void* arr[10];
		char** stack;
		char buf[BUF_SIZE];
		int fd = makeFIFO();
		snprintf(buf, BUF_SIZE, "[%lu] lock mutex from %lu", (unsigned long)mutex, pthread_self());
		for (int i = 0 ; i < BUF_SIZE;) i += write(fd, buf + i, BUF_SIZE) ;
		size_t sz = backtrace(arr, 10);
		stack = backtrace_symbols(arr, sz);
		snprintf(buf, BUF_SIZE, "#%s",stack[1]);
		for (int i = 0 ; i < BUF_SIZE;) i += write(fd, buf + i, BUF_SIZE) ;
		close(fd);
		unlock_cp(&mtx);


	}

	n_lock -= 1;
	return rtn;

}


int pthread_mutex_unlock(pthread_mutex_t* mutex){
	static __thread int n_unlock = 0;
	n_unlock += 1;

	if(unlock_cp == NULL) init_real_one();
	/*j
	int (*lock_cp)(pthread_mutex_t* mutex);
	char* error;
	lock_cp = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if((error = dlerror()) != 0x0) exit(1);
	*/
	int rtn = unlock_cp(mutex);

	if(n_unlock == 1){
		lock_cp(&mtx);
		int i;
		void* arr[10];
		char** stack;
		char buf[BUF_SIZE];
		int fd = makeFIFO();
		snprintf(buf, BUF_SIZE, "[%lu] unlock mutex from %lu", (unsigned long)mutex, pthread_self());
		for (int i = 0 ; i < BUF_SIZE;) i += write(fd, buf + i, BUF_SIZE) ;
		size_t sz = backtrace(arr, 10);
		stack = backtrace_symbols(arr, sz);
		snprintf(buf, BUF_SIZE, "#%s",stack[1]);
		for (int i = 0 ; i < BUF_SIZE;) i += write(fd, buf + i, BUF_SIZE) ;
		close(fd);
		unlock_cp(&mtx);


	}

	n_unlock -= 1;
	return rtn;

}


