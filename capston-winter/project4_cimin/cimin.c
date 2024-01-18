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

unsigned char *input = NULL;
unsigned char *error_keyword = NULL;
unsigned char *output = NULL;
unsigned char *target = NULL;
int args_size= 0;
char *args[16];
pid_t pid = -1;

int origin_size = 0;
int tm_size = 0;
unsigned char* tm = NULL;

int LETMESEE = 0;


void print(unsigned char* string, int len){
	for(int i = 0; i < len; i++) printf("%c", string[i]);
}

void save_result_as_file(){
	FILE* fp = fopen(output, "w");
	for(int i = 0; i < tm_size; i++) fprintf(fp, "%c", tm[i]);
	fclose(fp);
}

//make_result: print final size and save result as file
void make_result(){
	save_result_as_file();
	debug(
	);
	printf("\n===CIMIN=========================================\n");
	print(tm, tm_size);
	printf("\n=================================================\n\n");
	printf("(%d) -> (%d)\n", origin_size, tm_size);
	printf("reduced crash input saved in file \"%s\"\n", output);

}


//quit: signal handler for SIGINT
void quit(int sig){
	fprintf(stderr, "program terminate...\n");
	make_result();
	exit(EXIT_SUCCESS);
}

//timeout: signal handler for SIGALARM
void timeout(int sig){
	fprintf(stderr, "TIMEOUT OCURRED!\n");
    kill(pid, SIGTERM);
	quit(0);
}

//printUsage: print usage
void printUsage(unsigned char* binary){
	fprintf(stderr, "Usage: %s -i [crashed_input_filepath] -m [error_msg_keyword] \n	-o [output_filepath] [target_binary_name] (target_arg) ...\n", binary);
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
                output = optarg;
                break;
			case '?':	
			default:
				printUsage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

	if (input == NULL || error_keyword == NULL || output == NULL) {
		printUsage(argv[0]);
	    exit(EXIT_FAILURE);
	}
    for (int i = optind; i < argc; i++) {
		if(i == optind) target = argv[i];
		args[args_size++] = argv[i]; 
    }
	args[args_size] = NULL;

	if(strstr(args[0], " ") != NULL){
		unsigned char* tok = (args[args_size++] = strtok(args[0], " "));
		while(tok != NULL) tok = (args[args_size++] = strtok(NULL, " "));
	}

	debug(
		printf("Input: %s\n", input);
    	printf("Error Keyword: %s\n", error_keyword);
    	printf("Output: %s\n", output);
    	printf("Target: %s\n", target);
		for(int i = 0; i < args_size; i++) printf("args %d: %s\n", i+1, args[i]);
		printf("\n");
	);
	return;

}


//is_crashed: exec target program with input crash as stdin.
//return 0 if theres no crash, return 1 if there was crash
int is_crashed(unsigned char* crash, int size){

	int p2c[2], c2p[2];
	if((pipe(p2c) < 0) || (pipe(c2p) < 0)){
		perror("pipe()");
		exit(EXIT_FAILURE);
	}

	pid = fork();

	if(LETMESEE) printf("[is_crashed: %d] 	make pipe...\n", LETMESEE);

	if(pid < 0){				//error
		perror("fork");
		exit(EXIT_FAILURE);
	}else if(pid > 0){			//parents
		debug(fprintf(stderr, "parent start\n"););
		close(p2c[READ]);
		close(c2p[WRITE]);
		alarm(3);

		//send crash input as stdin of child
		write(p2c[WRITE], crash, size);
		close(p2c[WRITE]);
	if(LETMESEE) printf("	[is_crashed: %d] parents write...\n", LETMESEE);
		waitpid(pid, NULL, 0);
		alarm(0);

		//read output from child stderr
		unsigned char buf[BUF_SIZE];
		int len, error;
		while((len = read(c2p[READ], buf, BUF_SIZE-1)) > 0){
			buf[len] = 0;
			debug(printf("\nparents recv: %s\n", buf););
			//determine if error keyword we find is exist
			if(strstr(buf, error_keyword) != NULL) error = 1;
			else error = 0;
		}
	if(LETMESEE) printf("	[is_crashed: %d] parents read...\n", LETMESEE);
		close(c2p[READ]);

		debug(fprintf(stderr, "parent end\n"););
		return error;

	}else{						//child
		debug(fprintf(stderr, "child start\n"););
		close(p2c[WRITE]);
		close(c2p[READ]);

		//redirect some std pipes to our unnamed pipes
		dup2(p2c[READ], STDIN_FILENO);
		dup2(c2p[WRITE], STDERR_FILENO);
		close(STDOUT_FILENO); 
	if(LETMESEE) printf("	[is_crashed] child dup...\n");

		unsigned char buf[BUF_SIZE];
		execv(target, args);

		close(p2c[READ]);
		close(c2p[WRITE]);
		debug(fprintf(stderr, "child end\n"););
		exit(EXIT_SUCCESS);

	}
	return 0;

}


//make_substr: make substring of src to dest, range of start-end index, and return size of substr
int make_substr(unsigned char* src, unsigned char* dest, int start, int end){
	int index = 0;
	for(int i = start; i < end; i++) dest[index++] = src[i];
	return index;

}

//cat_strings: concatnate second string to first string, and return first string
unsigned char* cat_strings(unsigned char* first, unsigned char* second, int firstsize, int secondsize){
	for(int i = 0; i < secondsize; i++) first[firstsize + i] = second[i];
	return first;
}


//reduce: reduce crash input and return it
unsigned char* reduce(unsigned char* origin, int len){

	//TODO idk if its right...
	unsigned char head[BUF_SIZE], tail[BUF_SIZE], mid[BUF_SIZE];
	/*
	unsigned char *head = (unsigned char*)malloc(sizeof(unsigned char)*len);
	unsigned char *tail = (unsigned char*)malloc(sizeof(unsigned char)*len);
	unsigned char *min = (unsigned char*)malloc(sizeof(unsigned char)*len);
	*/

	debug(printf("start to reduce size %d...\n", len););
	printf("start to reduce size %d...\n", len);
	save_result_as_file();
	const int POINT = 1729;


	int s = len - 1;
	//if(s < POINT) LETMESEE = s;

	while(s > 0){	
		debug(printf("\n## src: %s (%d)\n", origin, s););
		if(s%10 == 0) printf("	while with %d\n", s);
	if(LETMESEE) printf("	before check head+tail...\n");
		for(int i = 0; i <= (len - s); i++){
			int headlen = make_substr(origin, head, 0, i);
	if(LETMESEE) printf("	make head...\n");
			int taillen = make_substr(origin, tail, (s+i), len);
	if(LETMESEE) printf("	make tail...\n");
			debug(printf("%d)\n	head : %s \n	tail: %s\n", i, head, tail););
			cat_strings(head, tail, headlen, taillen);
	if(LETMESEE) printf("	meow meow...\n");
			if(headlen+taillen == 0) continue;
			debug(printf("	meow: %s\n", head););
			if(is_crashed(head, headlen+taillen)){
				tm = origin;
				tm_size = len;
			
				return reduce(head, headlen+taillen);
			}
	if(LETMESEE) printf("	check if crashed...\n");
		}
	if(LETMESEE) printf("	before check min...\n");
		for(int i = 0; i <= (len - s); i++){
			int midlen = make_substr(origin, mid, i, (i+s));
			if(midlen == 0) continue;
			debug(printf("	mid: %s\n", mid););
			if(is_crashed(mid, midlen)){
				tm = origin;
				tm_size = len;
			
				return reduce(mid, midlen);
			}
		}
		s--;
	}
	return origin;

}

//this is MAIN
int main(int argc, char *argv[]) {
	unsigned char buf[BUF_SIZE];

	parseArgs(argc, argv);
	signal(SIGALRM, timeout);
	signal(SIGINT, quit);

	//get initial crash input
	int fd = open(input, O_RDONLY);
	origin_size = read(fd, buf, BUF_SIZE);
	if(buf[origin_size-2] == '\n') buf[(origin_size--)-2] = 0;

/*
	unsigned char* origin = (unsigned char*) malloc(sizeof(unsigned char) * origin_size + 1);
	tm = origin;
	tm_size = origin_size;
	reduce(origin, origin_size);
*/

	reduce(buf, origin_size);
	make_result();
    return 0;
}




