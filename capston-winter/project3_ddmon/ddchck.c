#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
//Capston/capston-winter/project3_ddmon/ddchck.c

char target[20];


//makeFIFO: make FIFO and open, return its fd. need to close fd after use it.
int makeFIFO(){
	if(mkfifo(".ddtrace", 0666)){
		if(errno != EEXIST) {
			perror("fail to open fifo: ");
			exit(EXIT_FAILURE);
		}
	}
	int fd = open(".ddtrace", O_RDONLY | O_SYNC) ;
	return fd;
}

//print_info: print lock/unlock information - can save mutex and thread id
void print_info(int fd, char* hooking_info){

	char buf[128];
	unsigned long mutex_lu, thread_lu;
	sscanf(hooking_info, "[%lu] %s mutex from %lu", &mutex_lu, buf, &thread_lu);
	//printf("%s\n", hooking_info);
	printf("[%s]\n	mutex:  #%lu\n	thread: #%lu\n", buf, mutex_lu, thread_lu);

}

//print_linenum: parse and print backtraced info as linenum - can save linenum
void print_linenum(int fd, char* backtrace_info){

	char buf[128];
	int *pc;
	int linenum;
	
	sscanf(backtrace_info, "#./target(+%p)*", &pc);
	pc = (int*)((char*)pc - 4);

	sprintf(buf, "addr2line -e %s %p", target, pc);
	FILE* fp = popen(buf, "r");
	if((fp == NULL) ||(fgets(buf, sizeof(buf), fp) == NULL)) {
		perror("popen or fgets: ");
		return;
	}
	char* tok = strtok(buf, ":");
	tok = strtok(NULL, ":");

	sscanf(tok, "%d", &linenum);
	printf("	line:   #%d\n\n", linenum);
	pclose(fp);
	
}


int main(int argc, char* argv[]){

	//get argv as target binary name
	if (argc > 1) strcpy(target, argv[1]);
	else printf("[usage] ./ddchck [target.out]\n");

	//make and open FIFO
	int fd = makeFIFO();

	//get information from FIFO and print it(or can save it)
	while(1){
		char s[128];
		int  len;
		if((len = read(fd, s, 128)) == -1) break;
		if(len > 0){
			if(s[0] == '#') print_linenum(fd, s); 
			else print_info(fd, s);
		}
	}

	//close FIFO
	close(fd);
	return 0;
}

