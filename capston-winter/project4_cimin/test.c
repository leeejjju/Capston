#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	
	char buf[128];
	printf("your name? >> ");
	scanf("%s", buf);
	sleep(atoi(argv[1]));
	fprintf(stderr, "Hello, %s!\n", buf);

}
