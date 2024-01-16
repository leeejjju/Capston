#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#define BUF_SIZE 4096
#define READ 0
#define WRITE 1
#ifdef DEBUG
    #define debug(fn) fn
#else
    #define debug(fn)
#endif

char *input = NULL;
char *error_keyword = NULL;
char *output_filepath = NULL;
char *target = NULL;
int args_size= 0;
char *args[16];
pid_t pid = -1;




void timeout(int sig){
	fprintf(stderr, "timeout occured!\n");
    kill(pid, SIGTERM);
}

void quit(int sig){
	fprintf(stderr, "program terminate...\n");
	exit(EXIT_FAILURE);
}

void printUsage(char* binary){
	
	fprintf(stderr, "	Usage: %s -i [crash_input] -m [error_keyword] \n		-o [output_filepath] [target_binary] (target_arg) ...\n", binary);
}

//parseArgs: parse args and save it to corresponding variables
void parseArgs(int argc, char *argv[]){
	
	int opt;
    while ((opt = getopt(argc, argv, "i:m:o:")) != -1) {
        switch (opt) {
            case 'i':
                input = optarg;
                break;
            case 'm':
                error_keyword = optarg;
                break;
            case 'o':
                output_filepath = optarg;
                break;
			case '?':	
				break;
//				printUsage(argv[0]);
//                exit(EXIT_FAILURE);
        }
    }

	if (input == NULL || error_keyword == NULL || output_filepath == NULL) {
		printUsage(argv[0]);
	    exit(EXIT_FAILURE);
	}
    for (int i = optind; i < argc; i++) {
		if(i == optind) target = argv[i];
		args[args_size++] = argv[i]; 
    }
	args[args_size] = NULL;

	if(strstr(args[0], " ") != NULL){
		printf("blank included in args...\n");
		char* tok = (args[args_size++] = strtok(args[0], " "));
		while(tok != NULL) tok = (args[args_size++] = strtok(NULL, " "));
	}

	debug(
		printf("Input: %s\n", input);
    	printf("Error Keyword: %s\n", error_keyword);
    	printf("Output: %s\n", output_filepath);
    	printf("Target: %s\n", target);
		for(int i = 0; i < args_size; i++) printf("args %d: %s\n", i+1, args[i]);
		printf("\n");
	);
	return;

}


//is_crashed: exec target program with input crash as stdin.
//return 0 if theres no crash, return 1 if there was crash
int is_crashed(char* crash, size_t size){

	int p2c[2], c2p[2];
	if((pipe(p2c) < 0) || (pipe(c2p) < 0)){
		perror("pipe()");
		exit(EXIT_FAILURE);
	}

	pid = fork();
	if(pid < 0){				//error
		perror("fork");
		exit(EXIT_FAILURE);
	}else if(pid > 0){			//parents
		debug(fprintf(stderr, "parent start\n"););
		close(p2c[READ]);
		close(c2p[WRITE]);
		alarm(3);

		//send
		write(p2c[WRITE], crash, size);
		close(p2c[WRITE]);
		waitpid(pid, NULL, 0);
		alarm(0);

		//read output
		char buf[BUF_SIZE];
		int len, error;
		while((len = read(c2p[READ], buf, BUF_SIZE)) > 0){
			buf[len] = 0;
			debug(printf("\nparents recv: %s\n", buf););
			if(strstr(buf, error_keyword) != NULL) error = 1;
			else error = 0;
		}
		close(c2p[READ]);

		debug(fprintf(stderr, "parent end\n"););
		return error;

	}else{						//child
		debug(fprintf(stderr, "child start\n"););
		close(p2c[WRITE]);
		close(c2p[READ]);

		dup2(p2c[READ], STDIN_FILENO);
		dup2(c2p[WRITE], STDERR_FILENO);

		char buf[BUF_SIZE];
		execv(target, args);

		close(p2c[READ]);
		close(c2p[WRITE]);
		debug(fprintf(stderr, "child end\n"););
		exit(EXIT_SUCCESS);

	}

	return 0;




}






//this is MAIN
int main(int argc, char *argv[]) {
	
	parseArgs(argc, argv);
	signal(SIGALRM, timeout);
	signal(SIGINT, quit);

	
	char buf[BUF_SIZE];
	int fd = open(input, O_RDONLY);
	int len = 0;
	len = read(fd, buf, BUF_SIZE);

	if(is_crashed(buf, len)) printf("crash occred!!\n");
	else printf("everything is find\n");


    return 0;
}




