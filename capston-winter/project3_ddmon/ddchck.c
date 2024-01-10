#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//Capston/capston-winter/project3_ddmon/ddchck.c

int main(){

	if(mkfifo(".ddtrace", 0666)){
		if(errno != EEXIST){
			perror("fail to open fifo: ");
			exit(EXIT_FAILURE);
		}
	}

	int fd = open(".ddtrace", O_RDONLY | O_SYNC) ;
	while(1){
		char s[128];
		int  len;
		if((len = read(fd, s, 128)) == -1) break;
		if(len > 0) printf("%s\n", s);
	}
	close(fd);
	return 0;
}

