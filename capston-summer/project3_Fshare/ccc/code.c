    �  #include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h> 
#include <pthread.h> 
#define BUFSIZE 4096
#define HEADER_RECV sizeof()
#define HEADER_SEND 
 



int port;
char* path;

void* runner(void* arg){


    int conn = *(int*)arg;
    free(arg);

    char buf[BUFSIZE] ;
	char * data = 0x0, * orig = 0x0 ;
	int len = 0 ;
	int s = 0;

    //receive the command from server
	while ( (s = recv(conn, buf, BUFSIZE-1, 0)) > 0 ) {
		buf[s] = 0x0 ;
		if (data == 0x0) {
			data = strdup(buf) ;
			len = s ;
		}
		else {
			data = realloc(data, len + s + 1) ;
			strncpy(data + len, buf, s) ;
			data[len + s] = 0x0 ;
			len += s ;
		}

	}

    
    //send apropriate buf to client 
    char cmd[5];
    char srcPath[BUFSIZE];
    sscanf(data, "%s %s", cmd, buf);
    sprintf(srcPath, "%s/%s", path, buf);
    printf("> cmd: %s, filename: %s\n", cmd, srcPath);
    

    if(!strcmp(cmd, "list")){
        //send it 
    }else if(!strcmp(cmd, "get")){
        //open src file
        //read and send the contents of the file
    }else if(!strcmp(cmd, "put")){
        //recieve...
        //make new file
        //and write the contents from stream
    }else if(!strcmp(cmd, "show")){

        //open the file
        FILE* srcFile = NULL;
        if((srcFile = fopen(srcPath, "r")) == NULL){
            perror("[cannot make file]");
            return NULL;
        }

        //read it and write the contents to client
        while((s = fread(buf, 1, BUFSIZE-1, srcFile)) > 0){

            if(ferror(srcFile)){
                perror("[cannot read contents]");
                fclose(srcFile);
                return NULL;
            }

            buf[s] = 0x0;

            if(send(conn, buf, s, 0) <= 0){
                perror("[cannot send contents]");
                fclose(srcFile);
                return NULL;
            }

            if(feof(srcFile)) break;
        }
        fclose(srcFile);
    }else{
        //invalid
    }

    printf("done......\n");
	shutdown(conn, SHUT_WR) ;
	if (orig != 0x0) free(orig);
    close(conn);
    return NULL;
}


int main(int argc, char** argv){

    
    
    if(argc < 4){
        printf("[server] invalid command\n");
        printf("usasg: \n");
        return EXIT_FAILURE;
    }
    
    //TODO getopt 
    //-p port, -d directory, & <-whatisthis
    port = atoi(argv[2]); 
    path = (char*)malloc(sizeof(char)*strlen(argv[4]) + 1);
    strcpy(path, argv[4]); 
    printf("port: %d path: %s\n", port, path);


	int listen_fd, new_socket;
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 

	char buffer[BUFSIZE] = {0}; 

	listen_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
	if (listen_fd == 0)  { 
		perror("socket failed : "); 
        free(path);
		exit(EXIT_FAILURE); 
	}

	
	memset(&address, '0', sizeof(address)); 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY /* the localhost*/ ; 
	address.sin_port = htons(port); 
	if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed : "); 
        free(path);
		exit(EXIT_FAILURE); 
	} 

	while (1) {
         
		if (listen(listen_fd, 16 /* the size of waiting queue*/) < 0) { 
			perror("listen failed"); 
            free(path);
			exit(EXIT_FAILURE); 
		} 

        new_socket = accept(listen_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen) ;
		if (new_socket < 0) {
			perror("accept"); 
            free(path);
			exit(EXIT_FAILURE); 
		} 

        int* passing_sock  = (int*)malloc(sizeof(int));
        (*passing_sock) = new_socket;
        pthread_t client_pid;
        if(pthread_create(&client_pid, NULL, (void*)runner, (void*)passing_sock)){
            perror("make new thread");
            free(path);
            close(new_socket);
        }
        
	}
    

    free(path);
    return EXIT_SUCCESS;

}