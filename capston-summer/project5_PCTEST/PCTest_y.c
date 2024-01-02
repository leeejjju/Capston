#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>

#define BUF_SIZE 128

int pipes1[2];
int pipes2[2];

void
compile_proc(char * target)
{
  char * input = (char *)malloc(strlen(target)-2);
  strncat(input, target, strlen(target)-2);
  execlp("gcc", "gcc", target, "-o", input, (char*)NULL);
}

int 
main(int argc, char *argv[])
{
  char * solution;
  char * target;
  char * testdir;
  int timeout;
  int exit_code;
  int correct, incorrect = 0;
  int tmpfd;
  int targetfd, solutionfd;
  pid_t cp_solution;
  pid_t cp_target;
  FILE *testfile;
  FILE *targetfp;
  FILE *solutionfp;


  //get options
	if (argc != 7){
		fprintf(stderr, " invalid command!\n\n Usage: %s -i <testdir> -t <timeout> <solution> <target>)\n\n",argv[0]);
		exit(EXIT_FAILURE);
	}
  const char *optstring = "i:t:";
  char option;
  optind = 1;
  while ( -1 != (option = getopt(argc, argv, optstring))){
    switch(option){
      case 'i':
        testdir = (char *)malloc(strlen(optarg)+1);
        strcpy(testdir, optarg);
        testdir[strlen(optarg)] = '\0';
        char test[] = "-t\0";
        if(strlen(testdir) == 0){
          fprintf(stderr, "There is no option for <testdir>");
          free(testdir);
		      exit(EXIT_FAILURE);
        }
        break;
      case 't':
        if(strlen(optarg) == 0){
          fprintf(stderr, "There is no option for <timeout>");
		      exit(EXIT_FAILURE);
        }
        timeout = atoi(optarg);
        break;
      case '?':
        break;
    }
  }
  solution = (char *)malloc(strlen(argv[5])+1);
  strcpy(solution,argv[5]);
  solution[strlen(argv[5])] = '\0';
  target = (char *)malloc(strlen(argv[6])+1);
  strcpy(target,argv[6]);
  target[strlen(argv[6])] = '\0';

  //Compile target and solution programs
  if((cp_solution = fork()) == -1){
    fprintf(stderr, "Fork error");
    goto Exit;
  }
  if (cp_solution == 0) {
		compile_proc(solution);
	}
  if((cp_target = fork()) == -1){
    fprintf(stderr, "Fork error");
    goto Exit;
  }
  if (cp_target == 0) {
		compile_proc(target);
	}
  waitpid(cp_solution, NULL, 0);
  waitpid(cp_target,&exit_code, 0);
  if(WIFEXITED(exit_code)){
    if(WIFSIGNALED(exit_code)){
      fprintf(stderr, "compile process: %d",WTERMSIG(exit_code));
      goto Exit;
    }
  }

  DIR *dir_info;
  struct dirent *dir_entry;
  if((dir_info = opendir(testdir)) == NULL) {
    fprintf(stderr, "Cannot read %s directory info.\n", testdir);
    free(testdir);
    goto Exit;
  }

  while((dir_entry = readdir(dir_info)) != NULL){
    if(pipe(pipes1) != 0){
      fprintf(stderr, "Pipe Error\n");
      goto Exit;
    }
    if(pipe(pipes2) != 0){
      fprintf(stderr, "Pipe Error\n");
      goto Exit;
    }
    if((strcmp(dir_entry->d_name,".") == 0) ||(strcmp(dir_entry->d_name,"..") == 0)){
      continue ;
    }
    char * filename = (char *)malloc(sizeof(dir_entry->d_name)+sizeof(testdir)+2);
    strcpy(filename, "./");
    strcat(filename,testdir);
    strcat(filename,"/");
    strcat(filename,dir_entry->d_name);
    //make child process to test target
    if((cp_target = fork())<0){
      fprintf(stderr, "fork error");
      free(filename);
      goto Exit;
    }
    if((cp_solution = fork())<0){
      fprintf(stderr, "fork error");
      free(filename);
      goto Exit;
    }
    if(cp_target == 0){
      close(pipes1[0]);
      int fd = open(filename, O_RDONLY);
      dup2(fd,STDIN_FILENO);
      close(fd);
      dup2(pipes1[1],STDOUT_FILENO);
      close(pipes1[1]);
      execl("./target", "target", (char*)NULL);
    }
    if(cp_solution == 0){
      close(pipes2[0]);
      int fd = open(filename, O_RDONLY);
      dup2(fd,STDIN_FILENO);
      close(fd);
      dup2(pipes2[1],STDOUT_FILENO);
      close(pipes2[1]);
      execl("./solution", "solution", (char*)NULL);
    }
    else{
      close(pipes1[1]);
      close(pipes2[1]);
      char buf[BUF_SIZE];
      int len;
      if ((targetfp = tmpfile( )) == NULL){
        perror("Cannot make a temporary file");
      }
      if ((solutionfp = tmpfile( )) == NULL){
        perror("Cannot make a temporary file");
      }
      targetfd = fileno(targetfp);
      solutionfd = fileno(solutionfp);
      while((len = read(pipes1[0],buf,sizeof(buf)))!=0){
        buf[len] = 0;
        //TODO: 끝까지 쓰는 것 보장하기
        int written = 0;
        while(written < len){
          written += write(targetfd,buf,len - written);
        }
      }
      while((len = read(pipes2[0],buf,sizeof(buf)))!=0){
        buf[len] = 0;
        //TODO: 끝까지 쓰는 것 보장하기
        int written = 0;
        while(written < len){
          written += write(targetfd,buf,len - written);
        }
      }
      waitpid(cp_target,&exit_code,0);
      waitpid(cp_solution,&exit_code,0);
      close(pipes1[0]);
      close(pipes2[0]);
    }
    free(filename);
  }
  closedir(dir_info);
  rewind(targetfp) ;
  rewind(solutionfp) ;
  //TODO: solution과 target의 output비교 및 출력

  fclose(targetfp);
  fclose(solutionfp);


  free(testdir);
  free(solution);
  free(target);
  return 0;

  Exit:
    free(testdir);
    free(solution);
    free(target);
    return 1;
}