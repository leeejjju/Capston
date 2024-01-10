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


int makeFIFO(){
	
	if(mkfifo("channel", 0666)){
		if(errno != EEXIST) {
			perror("fail to open fifo: ");
			exit(EXIT_FAILURE);
		}
	}

	int fd = open(".ddtrace", O_WRONLY | O_SYNC);
	return fd;

}


int pthread_mutex_lock (pthread_mutex_t* mutex){

	int (*lock_cp)(pthread_mutex_t*) = NULL ;
	char* error;
	lock_cp = dlsym(RTLD_NEXT, "pthread_mutex_lock") ;
	if ((error = dlerror()) != 0x0) exit(1) ;
	lock_cp(mutex) ;


	//TODO make mossikkang?
	char buf[50];
	int fd = makeFIFO();
	snprintf(buf, 50, "lock mutex %lu\n", (unsigned long)mutex);
	/*
	for (int i = 0 ; i < 50 ; ) {
		i += write(fd, buf + i, 50) ;
	}
	*/
	write(fd, buf, 50) ;

	return 0; 

}

int pthread_mutex_unlock (pthread_mutex_t *mutex){
	
	int (*unlock_cp)(pthread_mutex_t*) = NULL ;
	char * error ;
	unlock_cp = dlsym(RTLD_NEXT, "pthread_mutex_unlock") ;
	if ((error = dlerror()) != 0x0) exit(1) ;
	unlock_cp(mutex) ;

	char buf[50];
	int fd = makeFIFO();
	sprintf(buf, "unlock mutex %lu\n", (unsigned long)mutex) ;
	/*
	for (int i = 0 ; i < 50 ; ) {
		i += write(fd, buf + i, 50) ;
	}
	*/
	write(fd, buf, 50) ;

	return 0;
}


