#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#define DEBUG

/* NOFIX --- */

typedef enum {
	start, //0
	left, //1
	right, //2
	up, //3
	down, //4
	quit, //5
	N_op  //6
} commands ;

typedef enum {
	vertical, //0
	horizontal  //1
} direction ;

typedef struct {
	int id ;
	int y1, y2 ;	// y1: the minimum of y, y2: the maximum of y
	int x1, x2 ;	// x1: the minimum of x, x2: the maximum of x
	int span ;		// the number of cells 
	direction dir ;	// the direction of the car
} car_t ;

char * op_str[N_op] = {
	"start", //0
	"left", //1
	"right", //2
	"up", //3
	"down", //4
	"quit" //5
};

int n_cars = 0 ;
car_t * cars = 0x0 ;
int cells[6][6] ; // cells[Y][X]
// A6 -> cells[5][0]
// F4 -> cells[3][5]
// F1 -> cells[0][5]

/* --- NOFIX */


/*
!TIL

한 개념을 완전히 깔끔하게 끝내고 다음으로. complexity를 계속 줄인다. 헷갈리지 않게!!! 
본인부터 헷갈리면 남이랑 어케 협업하냐. 

현업에서 실력의 척도 -> 본인의 커밋이 메인 브랜치에 들어가기까지의 기간. 3개월이면 잘한거고 일년이면 짤림.
본인도 헷갈리는 코드는 코드리뷰도 걸러진다. 

NULL && 하면 false? 

클린코딩 -> 확실하게 코드가 나와야 고쳐쓸수도 있고... 정확한 시스템과 기능을 알아야 이게 가능함. 
머릿속에 확실하게 알고 있어야 길게 생각할 수 있다. 잘 모르면 상상을 못함 -> 버그터지고 진도 안 나가고 ... 

memset은 비싼 오퍼레이션 -> 중요치 않은 정보는 보안 신경 x ? 

무지성 찍어보기/디버그 대신 로직을 제대로 살필 것. 
코드를 보고 디버깅하기!!!
디버거를 켜야 하는 경우: 내가 짜지 않은 부분에서 오는 리턴값을 봐야 할 때 

GDP ? unix기반 라이브러리를 쓸 수 있나? 


fuzzing
load file 테스트


main(int argc, char ** argv)

argument로 파일명을 받아서 load_game에 넣기 

afl-gcc: fuzzing의 오픈소스 기본 도구 
afl-fuzz i input/ -o output/ ./a.out @@(랜덤파일) 
인풋폴더에 있는 파일들을 베이스로 랜덤값들을 만듬. 


path branch의 시퀀스? 경우의 수?

리빌드 후 output폴더에 저장되는 파일을 인풋으로 돌리면 fault가 듬. 크래시.
cat하면 내용 읽어볼 수 있음. 

대부분의 보안 결함은 입력 파싱하는 부분. 

*/


// return the corresponding number for the command given as s.
commands get_op_code (char * s){
	int i;
	for(i = 0; i < 6; i++){
		if(strcmp(op_str[i], s) == 0) break;
	}
	return (commands)(i);
}


// read input file and allocate cars 
// load_game returns 0 for a success, or return 1 for a failure.
int load_game (char * filename){
	// Use fopen, getline, strtok, atoi, strcmp

	FILE* fp = NULL;
	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "[error] cannot find the file %s\n", filename);
		return 1;
	}

	char* line = NULL;
	size_t len = 0;
	int i = 0;
	char cell[10], direction[10];
	int span; 

	//get the first line - num of cars
	getline(&line, &len, fp);
	line[strlen(line)-1] = 0;
	
	for(int k = 0; k < strlen(line); k++){
		if(!isdigit(line[k])){
			fprintf(stderr, "[error] invalid type of n_cars : %c\n", line[k]);
			fclose(fp);
			return 1;
		}
	}

	n_cars = atoi(line);
	
	if((n_cars < 2) || (n_cars > 35)){
		fprintf(stderr, "[error] invalid range of n_cars\n");
		return 1;
	}

	//allcate the space for cars 
	cars = (car_t*)malloc(sizeof(car_t)*n_cars);
	
	//get the information about each cars
	while(getline(&line, &len, fp) != -1){
		line[strlen(line)-1] = 0;
		char tmp[20];
		#ifdef DEBUG
		printf("[debug] car%d : %s\n", i+1, line);
		#endif 

		strcpy(cell, strtok(line, ":"));
		strcpy(direction, strtok(NULL, ":"));
		strcpy(tmp, strtok(NULL, ":"));
		
		if(cell == NULL || direction == NULL || tmp == NULL){
			fprintf(stderr, "[error] invalid file format\n");
			return 1;
		}
		if(isdigit(span)){
			fprintf(stderr, "[error] invalid file format\n");
			return 1;
		}
		span = *(tmp) - '0';
		cars[i].id = i+1;

		//check the position of c1 (red car)
		if(i == 0 && !(!strcmp(direction, "horizontal") && cell[1] == '4')){
			fprintf(stderr, "[error] invalid position of car1\n");
			fclose(fp);
			return 1;
		}


		//check the pivot
		if((strlen(cell) != 2) || !(cell[0] >= 'A' && cell[0] <= 'F') || !(cell[1] >= '1' && cell[1] <= '6')){
			fprintf(stderr, "[error] invalid cell value\n");
			fclose(fp);
			return 1;
		}else{
			cars[i].x1 = (cell[0]- 'A');
			cars[i].x2 = (cell[0]- 'A');
			cars[i].y1 = (cell[1] - '0') -1;
			cars[i].y2 = (cell[1] - '0') -1;
		}

		//check the span
		if(span < 1 || span > 6){
			fprintf(stderr, "[error] invalid span value\n");
			fclose(fp);
			return 1;
		}else{
			cars[i].span = span;
		}

		//check the direction and fill the values
		if(!strcmp(direction, "vertical")){
			cars[i].dir = vertical;
			cars[i].y2 = cars[i].y2 - span + 1;
			if(cars[i].y2 > 5){
				fprintf(stderr, "[error] invalid range of car%d\n", i);
				fclose(fp);
				return 1;
			}
		}else if(!strcmp(direction, "horizontal")){
			cars[i].dir = horizontal;
			cars[i].x2 = cars[i].x2 + span - 1;
			if(cars[i].x2 < 0){
				fprintf(stderr, "[error] invalid range of car%d\n", i);
				fclose(fp);
				return 1;
			}
		}else {
			fprintf(stderr, "[error] invalid direction\n");
			fclose(fp);
			return 1;
		}

		
		#ifdef DEBUG
		printf("[debug] token : %d:%d, %d:%d, %d, %d \n", cars[i].y1, cars[i].x1, cars[i].y2, cars[i].x2, cars[i].dir, cars[i].span);
		#endif 

		if(++i > n_cars){
			fprintf(stderr, "[error] wrong num of cars\n");
			fclose(fp);
			return 1;
		}

	}
	
	fclose(fp);
	return 0;
	
}


//display the current cells array in format
void display (){
	/* The beginning state of board1.txt must be shown as follows: 
 	 + + 2 + + +
 	 + + 2 + + +
	 1 1 2 + + +
	 3 3 3 + + 4
	 + + + + + 4
	 + + + + + 4
	*/
	printf(" \\ ");
	for(int i = 0; i < 6; i++){
		printf("%c ", 'A'+i);
	}
	printf("\n");

	for(int y = 5; y >= 0; y--){
		printf("%d: ", y+1);
		for(int x = 0; x < 6; x++){
			if(cells[y][x]) printf("%d ", cells[y][x]); 
			else printf("+ ");
		}
		printf("\n");
	}
	printf("\n");
}



// Update cars[id].x1, cars[id].x2, cars[id].y1 and cars[id].y2 according to the given command (op) if it is possible.
// move returns 1 when the given input is invalid / return 0 for a success.
// TODO car관련만 다루도록
//TODO car(모델) -> cell(뷰)로 이어지는 흐름. view의 보장이 없으므로 확실한 분리 - 함수와 관련 없는 변수를 고려할 필요 없도록
		
int move (int id, int op){
	int index = id - 1;  //한 개념이 여러 용도로 쓰이면 안 됨

	switch (op){
	case left:
		//check the direction
		if(cars[index].dir == vertical){
			return 1;
		}

		//check the area 
		if((cars[index].x1-1 < 0) || cells[cars[index].y1][cars[index].x1-1] != 0){
			return 1;
		}

		//move and update
		cars[index].x1--;
		cars[index].x2--;

		break;

	case right:
		//check the direction
		if(cars[index].dir == vertical){
			return 1;
		}

		//check the area 
		if((cars[index].x2+1 > 5) || cells[cars[index].y2][cars[index].x2+1] != 0){
			return 1;
		}

		//move and update
		cars[index].x1++;
		cars[index].x2++;
		break;

	case down:
		//check the direction
		if(cars[index].dir == horizontal){
			return 1;
		}

		//check the area 
		if((cars[index].y2-1 < 0) || cells[cars[index].y2-1][cars[index].x2] != 0){
			return 1;
		}

		//move and update
		cars[index].y1--;
		cars[index].y2--;
		break;

	case up:
		//check the direction
		if(cars[index].dir == horizontal){
			return 1;
		}

		//check the area 
		if((cars[index].y1+1 > 5) || cells[cars[index].y1+1][cars[index].x1] != 0){
			return 1;
		}

		//move and update
		cars[index].y1++;
		cars[index].y2++;
		break;	

	}

	return 0;

	// The condition that car_id can move left is when 
	//  (1) car_id is horizontally placed, and
	//  (2) the minimum x value of car_id is greather than 0, and
	//  (3) no car is placed at cells[cars[id].y1][cars[id].x1-1].
	// You can find the condition for moving right, up, down as a similar fashion.

}


// update cells array by information of cars array
// return 0 for sucess / return 1 if the given car information (cars) has a problem
int update_cells (){
	memset(cells, 0, sizeof(int) * 36) ; // clear cells before the write.

	for(int i = 0; i < n_cars; i++){

		if(cars[i].dir == 0){
			for(int k = 0; k < cars[i].span; k++){
				if(!cells[cars[i].y1 - k][cars[i].x1]) cells[cars[i].y1 - k][cars[i].x1] = cars[i].id;
				else  return 1;
			} 

		}else if(cars[i].dir == 1){
			for(int k = 0; k < cars[i].span; k++){
				if(!cells[cars[i].y1][cars[i].x1 + k]) cells[cars[i].y1][cars[i].x1 + k] = cars[i].id;
				else return 1;
			}
		}
	}

	return 0;

}

// print success msg and terminate program
void success(){
	printf("\n*..☆'.｡.:*.SUCCESS･*..☆.'｡.:*.\n\n");
	display();
	printf(".｡.:.+*:ﾟ+｡.ﾟ･*..☆.｡.:*.☆.｡.:'\n\n");
	exit(EXIT_SUCCESS);
}

int main (){
	char buf[128] ;
	int op ;
	int id ;

	// //! for test
	// strcpy(buf, "error.txt");
	// #ifdef DEBUG
	// printf("[debug] start the game with file %s\n", buf);
	// #endif
	// load_game(buf) ;
	// update_cells() ;
	// //! for test

	while (1) {
		
		// get the first command
		scanf("%s", buf) ;
		getchar();

		switch (op = get_op_code(buf)) {

			case start:
				//get filename
				scanf("%s", buf) ;
				getchar();

				if(load_game(buf)){
					printf("[RushHourPuzzle] invalid file\n");
					continue;
				}

				if(update_cells()){
					printf("[RushHourPuzzle] invalid file\n");
					fprintf(stderr, "[error] duplicated cars on cells\n");
					continue;
				}
				display();
				break;

			case left:
			case right:
			case up:
			case down:
				if(cars == NULL){
					printf("[RushHourPuzzle] please start the game first! \n");
					getchar();
					continue;
				}

				//get the number i 
				scanf("%s", buf) ;
				getchar();

				if(!sscanf(buf, "%d", &id) || id < 1 || id > n_cars){
					printf("[RushHourPuzzle] invalid car number\n");
					continue;
				}

				if(move(id, op)){
					printf("[RushHourPuzzle] impossible! \n");
					continue;
				}
				if(update_cells()) {
					fprintf(stderr, "[error] duplicated cars on cells\n");
					continue;
				}
				display();
				break;
			case quit:
				printf("[RushHourPuzzle] game terminated\n");
				return 0;
			default:
				printf("[RushHourPuzzle] invalid command\n");
				// getchar();

		}

		if(cells[3][5] == 1){
		success();
	}

	}

}
