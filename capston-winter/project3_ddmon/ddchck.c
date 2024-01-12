#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#define MAX_MUTEX_NUM 10
//Capston/capston-winter/project3_ddmon/ddchck.c

typedef struct Mutex{
    unsigned long m_id;
    struct Mutex* next;
    struct Mutex* prev;
}mutex_t;

typedef struct Thread{
    unsigned long t_id;
    mutex_t* mutex_head;
    mutex_t* mutex_tail;
    struct Thread* next;
}thread_t;

typedef struct Edge{
    unsigned long start;
    unsigned long end;
    struct Edge * next;
    struct Edge * prev;
}edge_t;

thread_t * thread_head = NULL;
edge_t * edge_head = NULL;
edge_t * edge_tail = NULL;
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

//print_linenum: parse and print backtraced info as linenum - return linenum
int print_linenum(int fd, char* backtrace_info){

	char buf[128];
	int *pc;
	int linenum;
	
	sscanf(backtrace_info, "#./target(+%p)*", &pc);
	pc = (int*)((char*)pc - 4);

	sprintf(buf, "addr2line -e %s %p", target, pc);
	FILE* fp = popen(buf, "r");
	if((fp == NULL) ||(fgets(buf, sizeof(buf), fp) == NULL)) {
		perror("popen or fgets: ");
		return -1;
	}
	char* tok = strtok(buf, ":");
	tok = strtok(NULL, ":");

	sscanf(tok, "%d", &linenum);
	pclose(fp);

	return linenum;
	
}

//insertMutex: insert mutex node to input thread's linked list
void insertMutex(thread_t * thread, unsigned long m_id) {
    mutex_t* newMutex = (mutex_t*)malloc(sizeof(mutex_t));
    // printf("%lu\n",newData);
    newMutex->m_id = m_id;
    newMutex->next = NULL;

    if( thread->mutex_head == NULL) {
        thread->mutex_head = newMutex;
        newMutex->prev = NULL;
    }
    else
    {
        newMutex->prev = thread->mutex_tail;
        thread->mutex_tail->next = newMutex;
    }

    thread->mutex_tail = newMutex;

}

//deleteMutex: delete mutex from input thread's linked list
void deleteMutex(thread_t * thread, mutex_t * current) {

    if(current->prev == NULL && current->next == NULL)
    {
        thread->mutex_head = NULL;
        thread->mutex_tail = NULL;
        
    }
    // 삭제할 노드가 리스트의 첫 번째 노드인 경우
    else if (current->prev == NULL) {
        current->next->prev = NULL;
        thread->mutex_head = current->next;
    } 
    // 삭제할 노드가 리스트의 마지막 노드인 경우
    else if (current->next == NULL) {
        current->prev->next = NULL;
        thread->mutex_tail = current->prev;
    }
    else
    {
        current->prev->next = current->next;
        current->next->prev = current->prev;
    }
    free(current);
}


//TODO 가능하면 delete 할 때  visited edge도 업애면 좋을듯...
//is_cycle_exist: check if cycle is exist from the mutex edges
int is_cycle_exist()
{   
    int edge_count = 0;
    //relloc 하는 방식으로..?
    edge_t visited[100];

    thread_t * thread_temp = thread_head;

    do
    {   
        
        if(thread_temp == NULL)
            break;
        mutex_t * mutex_temp = thread_temp->mutex_head;
        do
        {
            if(mutex_temp == NULL)
                break;
            
            if(mutex_temp->next != NULL)
            {
                edge_t edge = {mutex_temp->m_id, mutex_temp->next->m_id};
                // printf("edge start : %lu edge end : %lu\n",edge.start, edge.end);

                // int dup_flag = 0;
                for(int i=0; i<edge_count; i++)
                {
                    // printf("visit start : %lu visit end : %lu\nedge start : %lu edge end : %lu\n",visited[i].start, visited[i].end, edge.start, edge.end);
                    
                    //cycle
                    if(((visited[i].start == edge.start) && (visited[i].end == edge.end)) || ((visited[i].start == edge.end) && (visited[i].end == edge.start)))
                    {
                        return 1;
                    }
                    //v:(a,b) e:(c,a)
                    if(visited[i].start == edge.end)
                    {
                        visited[edge_count].start = edge.start;
                        visited[edge_count].end = visited[i].end;
                        edge_count++;
                    }
                    //v:(a,b) e:(b,c)
                    if(visited[i].end == edge.start)
                    {
                        visited[edge_count].start = visited[i].start;
                        visited[edge_count].end = edge.end;
                        edge_count++;
                    }
                }
                visited[edge_count].start = edge.start;
                visited[edge_count].end = edge.end;
                edge_count++;
            }
        }while((mutex_temp = mutex_temp->next) != NULL);
    }while((thread_temp = thread_temp->next) != NULL);

   return 0;
}


// mutex_lock: add mutex node to input thread's linked list
void mutex_lock(unsigned long t_id, unsigned long m_id)
{
    if(thread_head == NULL)
    {
        thread_head = (thread_t *)malloc(sizeof(thread_t));
        thread_head->t_id = t_id;
        thread_head->next = NULL;
        insertMutex(thread_head, m_id);
        return;
    }

    thread_t * thread_temp = thread_head;
    int exist_tid = 0;
    //thread가 존재하는지 안하는지 판단
    do
    {
        if(t_id == thread_temp->t_id)
        {
            exist_tid = 1;
            break;
        }
        if(thread_temp->next == NULL)
            break;
        thread_temp = thread_temp->next;
    }while(1);

    
    if(exist_tid)
    {
        insertMutex(thread_temp, m_id);
    }
    else
    {
        thread_t * new_thread = (thread_t *)malloc(sizeof(thread_t));
        new_thread->t_id = t_id;
        new_thread->next = NULL;
        insertMutex(new_thread, m_id);
        thread_temp->next = new_thread;
    }
    
}

//mutex_unlock: delete mutex from input thread's linked list
void mutex_unlock(unsigned long t_id, unsigned long m_id)
{
    thread_t * thread_temp = thread_head;

    do
    {   
        if(thread_temp == NULL)
            break;
        if(thread_temp->t_id == t_id)
        {
            mutex_t * mutex_temp = thread_temp->mutex_head;
            do
            {
                if(mutex_temp == NULL)
                    break;
                if(mutex_temp->m_id == m_id)
                {
                    deleteMutex(thread_temp, mutex_temp);
                    break;
                }
            }while((mutex_temp=mutex_temp->next) != NULL);
            
            break;
        }
    }while((thread_temp = thread_temp->next) != NULL);


}

//print_current: print the thread information from input thread to last thread
void print_thread_info(int sig)
//void print_current(thread_t * thread_start)
{
    thread_t * thread_temp = thread_head;
    
    while(1)
    {
        fprintf(stderr,"thread%lu has ",thread_temp->t_id);

        mutex_t * mutex_temp = thread_temp->mutex_head;
        if(mutex_temp == NULL)
        {   
            fprintf(stderr,"\n");
            if(thread_temp -> next == NULL)
                break;
            thread_temp = thread_temp -> next;
            continue;
        }
        while(1)
        {
            fprintf(stderr,"%lu ",mutex_temp->m_id);

            if(mutex_temp->next == NULL)
                break;
            mutex_temp = mutex_temp->next;
        }

        fprintf(stderr,"\n");

        if(thread_temp -> next == NULL)
            break;
        thread_temp = thread_temp -> next;
    }

	exit(EXIT_SUCCESS);
}

void print_deadlock_alert(int linenum){

	printf("\n================================\n\n");
	printf("    DEADLOCK OCCURED at line %d!!\n\n", linenum);
	printf("\n================================\n\n");

}

int main(int argc, char* argv[]){

	//get argv as target binary name
	if (argc > 1) strcpy(target, argv[1]);
	else printf("[usage] ./ddchck [target.out]\n");

	signal(SIGINT, print_thread_info);

	//make and open FIFO
	unsigned long mutex_lu, thread_lu;
	int linenum;
	int cnt = 0;
	remove("./.ddtrace");

		int fd = makeFIFO();
	//get information from FIFO and lock or unlock mutex
	while(1){
		
		char s[128];
		char cmd[128];
		int  len;
		/*
*/
		if((len = read(fd, s, 128)) == -1) break;
		if(len > 0){
			if(s[0] == '#') {
				if(!strcmp(cmd, "lock")) {
					printf("[%lu] lock accuired   : %lu\n", thread_lu, mutex_lu);
					mutex_lock(thread_lu, mutex_lu);
					if(is_cycle_exist()) print_deadlock_alert(print_linenum(fd, s));
				}else{
					printf("[%lu] unlock accuired : %lu\n", thread_lu, mutex_lu);
					mutex_unlock(thread_lu, mutex_lu);
				}
			}else{
				sscanf(s, "[%lu] %s mutex from %lu", &mutex_lu, cmd, &thread_lu);
			}	
		}
		//if(cnt++ > 10) break;
    //mutex_lock(1,cnt++);
    //print_current(thread_head);
		//if(remove(".ddtrace") != 0) perror("unlink FIFO:");
	}

		close(fd);
	//lock -> mutex_lock -> is_cycle_exist -> print linenum
	//unlock ->mutex_unlock

	//close FIFO

/*
    mutex_lock(140057917003520,94236090622048);
    mutex_lock(140057917003520,94236090622049);
    mutex_unlock(140057917003520,94236090622048);
    mutex_unlock(140057917003520,94236090622049);

    mutex_lock(12341234123412,94236090622048);

    print_current();

    if(is_cycle_exist()) fprintf(stderr,"deadlock\n");
    else fprintf(stderr,"no problem :)\n");


*/
	printf("done..\n");

	return 0;
}



