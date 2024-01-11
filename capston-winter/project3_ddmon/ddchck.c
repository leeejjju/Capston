#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//Capston/capston-winter/project3_ddmon/ddchck.c

char target[20];

void print_linenum(int fd, void* pc){

	char buf[128];
	sprintf(buf, "addr2line -e %s %p", target, pc);
	printf("	%s\n", buf);

}


int main(int argc, char* argv[]){
 	if (argc > 1) {
		strcpy(target, argv[1]);
		printf("target: %s\n", target);
	}else{
		printf("[usage] ./ddchck [target.out]\n");
	}

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
		if(len > 0){
		//	if(s[0] == '#') print_linenum(fd, s); 
		//	else printf("%s\n", s);
			printf("%s\n", s);
		}
	}
	close(fd);
	return 0;
}

