#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gtk/gtk.h>
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

// v2_new --- 
car_t* init_cars = NULL;
GtkWidget* window;
GtkWidget *mat_main, *mat_solve, *mat_answer;
GtkWidget *mat_screen, *mat_ans_btn, *mat_sol_btn, *mat_menu;
GtkWidget *btn_solve, *btn_answer, *btn_next, *btn_prev, *btn_exit, *btn_reset;
GtkWidget *btn_auto, *btn_up, *btn_down, *btn_left, *btn_right;
GtkWidget *label_info, *label_me;
char main_color[20] = "#FF9797";  
char btn_color[20] = "#F0EFEB";
char colors[6][20] = {"#FFC8A2", "#FCEDBE","#C9E4D9", "#BDDAE6", "#ECD5E3", "#F8BBD0"};

int selected_car_num = -1;
const int ANSWER = 0, SOLVE = 1;
const int UP = 3, DOWN = 4, LEFT = 1, RIGHT = 2;
const int PREV = 0, NEXT = 1;
char msg_info[128] = "";

GdkPixbuf *create_pixbuf(const gchar * filename);
void display_screen();
void add_mat_solve();
void add_mat_answer();
void change_mode(GtkWidget *widget, gpointer md);
void select_car(GtkWidget *widget, gpointer id);
void move_car(GtkWidget *widget, gpointer cmd);
void set_color(GtkWidget* widget, int color);
void show_state(GtkWidget* widget, gpointer p);
void exit_game(GtkWidget* widget);
void reset_game(GtkWidget* widget);

// --- v2_new

// ys_add ---

typedef struct DlistNode{
   struct DlistNode* llink;
   struct DlistNode* rlink;
   struct DlistNode* prev;
   struct DlistNode* next;
   car_t *state;
}DlistNode;

typedef struct HashNode{
   struct DlistNode* value;
   struct HashNode* next;
}HashNode;

DlistNode * Q_head = 0x0;
DlistNode * Q_tail = 0x0;
DlistNode * Final = 0x0;
DlistNode * Solution = 0x0;
DlistNode* curr = 0x0;
HashNode * hash_table[2000];


void init_node(DlistNode* head);
void insert_Q(DlistNode* newnode);
DlistNode* pop();
void free_all_nodes(DlistNode* head, DlistNode* tail);
int hash_function(DlistNode* node);
int check_hash(DlistNode* node);
void insert_H(DlistNode* node);
DlistNode* make_adjnode(DlistNode* cur);
DlistNode* find_adj(DlistNode* cur);
int find_path();
void make_solution();

int load_game (char * filename);
void display ();
int update_cells (car_t * car_state);

// --- ys_add

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
	init_cars = (car_t*)malloc(sizeof(car_t)*n_cars);
	
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
	memcpy(init_cars, cars, sizeof(car_t)*n_cars);
	return 0;
	
}


//display the current cells array in format
void display(){
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
int move(int id, int op){
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

/*
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
*/


// update cells array by information of cars array
// return 0 for sucess / return 1 if the given car information (cars) has a problem
int update_cells (car_t * car_state){
   memset(cells, 0, sizeof(int) * 36) ; // clear cells before the write.
   for(int i = 0; i < n_cars; i++){
      if(car_state[i].dir == 0){
         for(int k = 0; k < car_state[i].span; k++){
            if(!cells[car_state[i].y1 - k][car_state[i].x1]) cells[car_state[i].y1 - k][car_state[i].x1] = car_state[i].id;
            else  return 1;
         } 

      }else if(car_state[i].dir == 1){
         for(int k = 0; k < car_state[i].span; k++){
            if(!cells[car_state[i].y1][car_state[i].x1 + k]) cells[car_state[i].y1][car_state[i].x1 + k] = car_state[i].id;
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



// v2_new ---

//THIS IS MAIN
int main(int argc, char *argv []){
	char buf[128] ;
	int op ;
	int id ;

	printf("argc: %d\n", argc);

	if(argc < 2){
		printf("usage: ./game [board file]\n");
		exit(EXIT_FAILURE);
	}

	strcpy(buf, argv[1]);
	printf("loading board %s ...\n", buf);

	if(load_game(buf)){
		printf("[rushhourpuzzle] invalid file\n");
		exit(EXIT_FAILURE);
	}

	if(update_cells(cars)){
		printf("[rushhourpuzzle] invalid file\n");
		fprintf(stderr, "[error] duplicated cars on cells\n");
		exit(EXIT_FAILURE);
	}

	display();
	if(find_path()){
		exit(EXIT_SUCCESS);
	}else{
		update_cells(cars);;
	}


  gtk_init(&argc, &argv); //init GTK by args
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);//make window

  //set the window
  gtk_window_set_title(GTK_WINDOW(window), "rushhour v2");
  gtk_window_set_default_size(GTK_WINDOW(window), 480, 720);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(window), 5);

  //change the icon of page(for cuteness)
  GdkPixbuf *icon = create_pixbuf("catIcon.png");
  gtk_window_set_icon(GTK_WINDOW(window), icon);

  //add widgets...
  //set main matrix
  mat_main = gtk_vbox_new(FALSE, 5);
  gtk_container_add(GTK_CONTAINER(window), mat_main);

  display_screen();

  GtkWidget* line1 = gtk_hseparator_new();
  GtkWidget* line2 = gtk_hseparator_new();
  label_info = gtk_label_new(msg_info);
  gtk_widget_set_size_request(label_info, 360, 32);
  gtk_box_pack_start(GTK_BOX(mat_main), line1, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mat_main), label_info, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mat_main), line2, FALSE, FALSE, 0);

  add_mat_solve();

  gtk_widget_show_all(window); //is it dup with above
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);//for termination
  g_object_unref(icon);
  gtk_main(); //enter the GTK main loop
				
}


GdkPixbuf *create_pixbuf(const gchar * filename) {
   GdkPixbuf *pixbuf;
   GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);
   if (!pixbuf) {
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
   }
   return pixbuf;
}


void display_screen(){

  //set screen matrix
  if(mat_screen == NULL) {
  	mat_screen = gtk_table_new(6, 6, TRUE);
  	gtk_table_set_row_spacings(GTK_TABLE(mat_screen), 2);
  	gtk_table_set_col_spacings(GTK_TABLE(mat_screen), 2);
    gtk_widget_set_size_request(mat_screen, 360, 360);
  }else{
	 gtk_container_foreach(GTK_CONTAINER(mat_screen), (GtkCallback)gtk_widget_destroy, NULL);
  }
  gtk_container_set_border_width(GTK_CONTAINER(mat_screen), 10);

  GtkWidget *btn_tmp;
  for(int i = 0; i < n_cars; i++){
	gchar label_text[4];
	g_snprintf(label_text, sizeof(label_text), "%d", cars[i].id);
    btn_tmp = gtk_button_new_with_label(label_text);
	set_color(btn_tmp, cars[i].id-2);
	g_signal_connect(G_OBJECT(btn_tmp), "clicked", G_CALLBACK(select_car), &(cars[i].id));
	gtk_table_attach_defaults(GTK_TABLE(mat_screen), btn_tmp, cars[i].x1, cars[i].x2+1, 5-cars[i].y1, 5-cars[i].y2+1);
  }

  gtk_container_add(GTK_CONTAINER(window), mat_screen);
  gtk_box_pack_start(GTK_BOX(mat_main), mat_screen, TRUE, TRUE, 0);
  gtk_widget_show_all(window);

}



void add_mat_solve(){

  mat_solve = gtk_table_new(5, 4, TRUE);
  gtk_container_add(GTK_CONTAINER(window), mat_solve);
  gtk_table_set_row_spacings(GTK_TABLE(mat_solve), 5);
  gtk_table_set_col_spacings(GTK_TABLE(mat_solve), 5);

  btn_up = gtk_button_new_with_label("▲");
  btn_down = gtk_button_new_with_label("▼");
  btn_left = gtk_button_new_with_label("◀");
  btn_right = gtk_button_new_with_label("▶");
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), btn_up, 1, 3, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), btn_down, 1, 3, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), btn_left, 0, 2, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), btn_right, 2, 4, 1, 2);
  g_signal_connect(G_OBJECT(btn_up), "clicked", G_CALLBACK(move_car), (gpointer*)&UP);
  g_signal_connect(G_OBJECT(btn_down), "clicked", G_CALLBACK(move_car), (gpointer*)&DOWN);
  g_signal_connect(G_OBJECT(btn_left), "clicked", G_CALLBACK(move_car), (gpointer*)&LEFT);
  g_signal_connect(G_OBJECT(btn_right), "clicked", G_CALLBACK(move_car), (gpointer*)&RIGHT);
  set_color(btn_up, -2);
  set_color(btn_down, -2);
  set_color(btn_left, -2);
  set_color(btn_right, -2);

  btn_answer = gtk_button_new_with_label("show answer 💡");
  gtk_container_add(GTK_CONTAINER(window), btn_answer);
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), btn_answer, 0, 4, 3, 4); 
  g_signal_connect(G_OBJECT(btn_answer), "clicked", G_CALLBACK(change_mode), (gpointer*)&ANSWER);
  set_color(btn_answer, -2);
 
  
  mat_menu = gtk_table_new(2, 4, TRUE);
  gtk_container_add(GTK_CONTAINER(window), mat_menu);
  gtk_table_set_row_spacings(GTK_TABLE(mat_menu), 4);
  gtk_table_set_col_spacings(GTK_TABLE(mat_menu), 4);
  
  btn_exit = gtk_button_new_with_label("exit");
  btn_reset = gtk_button_new_with_label("reset");
  gtk_table_attach_defaults(GTK_TABLE(mat_menu), btn_exit, 0, 2, 0, 2); 
  gtk_table_attach_defaults(GTK_TABLE(mat_menu), btn_reset, 2, 4, 0, 2); 
  g_signal_connect(G_OBJECT(btn_exit), "clicked", G_CALLBACK(exit_game), NULL);
  g_signal_connect(G_OBJECT(btn_reset), "clicked", G_CALLBACK(reset_game), NULL);
  set_color(btn_exit, -2);
  set_color(btn_reset, -2);


  gtk_table_attach_defaults(GTK_TABLE(mat_solve), mat_menu, 0, 2, 4, 5);


  label_me = gtk_label_new("23-winter capston study#1 leeejjju");
  gtk_container_add(GTK_CONTAINER(window), label_me);
  gtk_misc_set_alignment(GTK_MISC(label_me), 1.0, 1.0);
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), label_me, 2, 4, 4, 5);

  gtk_box_pack_start(GTK_BOX(mat_main), mat_solve, TRUE, TRUE, 0);


} 

void add_mat_answer(){

  mat_answer = gtk_table_new(5, 4, TRUE);
  gtk_container_add(GTK_CONTAINER(window), mat_answer);
  gtk_table_set_row_spacings(GTK_TABLE(mat_answer), 5);
  gtk_table_set_col_spacings(GTK_TABLE(mat_answer), 5);

  btn_prev = gtk_button_new_with_label("◁ prev step");
  btn_next = gtk_button_new_with_label("next step ▷");
  gtk_table_attach_defaults(GTK_TABLE(mat_answer), btn_prev, 0, 2, 0, 3);
  gtk_table_attach_defaults(GTK_TABLE(mat_answer), btn_next, 2, 4, 0, 3);
  g_signal_connect(G_OBJECT(btn_prev), "clicked", G_CALLBACK(show_state), (gpointer*)&PREV);
  g_signal_connect(G_OBJECT(btn_next), "clicked", G_CALLBACK(show_state), (gpointer*)&NEXT);
  set_color(btn_prev, -2);
  set_color(btn_next, -2);
  
  btn_solve = gtk_button_new_with_label("back to solve ✎");
  gtk_container_add(GTK_CONTAINER(window), btn_solve);
  gtk_table_attach_defaults(GTK_TABLE(mat_answer), btn_solve, 0, 4, 3, 4); 
  g_signal_connect(G_OBJECT(btn_solve), "clicked", G_CALLBACK(change_mode), (gpointer*)&SOLVE);
  set_color(btn_solve, -2);
  
  label_me = gtk_label_new("23-winter capston study#1 leeejjju");
  gtk_container_add(GTK_CONTAINER(window), label_me);
  gtk_misc_set_alignment(GTK_MISC(label_me), 1.0, 1.0);
  gtk_table_attach_defaults(GTK_TABLE(mat_answer), label_me, 2, 4, 4, 5);

  gtk_box_pack_start(GTK_BOX(mat_main), mat_answer, TRUE, TRUE, 0);
  

} 

//get target mode
void change_mode(GtkWidget* widget, gpointer md){
	int i = 0;
	
	int mode = *(int*)md;
	if(mode == ANSWER){
		strcpy(msg_info, "Changed mode to answer");

		find_path();
		if(Solution == NULL) g_print("no answer...\n");
		curr = Solution;
		cars = curr->state;

		gtk_widget_destroy(mat_solve);
		add_mat_answer();

	}else if(mode == SOLVE){
		strcpy(msg_info, "Changed mode to solve");

		gtk_widget_destroy(mat_answer);
		add_mat_solve();

	}

	g_print("this part is excuted...\n");
	gtk_label_set(GTK_LABEL(label_info), msg_info);
	gtk_widget_show_all(window);

}

void select_car(GtkWidget* widget, gpointer id){

	int* p = (int*) id;
	selected_car_num = *p;
	sprintf(msg_info, "%d selected...", selected_car_num);
	
    gtk_label_set(GTK_LABEL(label_info), msg_info);
	gtk_widget_show_all(window);

}


void move_car(GtkWidget *widget, gpointer cmd){

	int* command = (int*) cmd;	
	if(selected_car_num == -1){
		strcpy(msg_info, "Please select the car first!");
    	gtk_label_set(GTK_LABEL(label_info), msg_info);
		gtk_widget_show_all(window);
		return;
	}

	if(move(selected_car_num, *command)){
		strcpy(msg_info, "Impossible movement!");
		gtk_label_set(GTK_LABEL(label_info), msg_info);
		gtk_widget_show_all(window);
		return;
	}
	

	if(update_cells(cars)) {
		strcpy(msg_info, "Impossible movement!");
		gtk_label_set(GTK_LABEL(label_info), msg_info);
		gtk_widget_show_all(window);
		return;
	}

	display_screen();

	if(cells[3][5] == 1){
		strcpy(msg_info, " ･*..☆.'｡.:*.ദ്ദി ˃  ᴗ  ˂   ) *..☆'.｡.: { SUCCESS } ..☆.'｡.:*.ദ്ദി ❁ ´ ◡ ` ❁ ) )*..☆'.｡.:*.");
		gtk_label_set(GTK_LABEL(label_info), msg_info);
		gtk_widget_show_all(window);
		//sleep(5);
		//exit(EXIT_SUCCESS);
	}else{
		sprintf(msg_info, "Moved car %d to %s.", selected_car_num, op_str[*command]);
    	gtk_label_set(GTK_LABEL(label_info), msg_info);
	}
	gtk_widget_show_all(window);

}


void set_color(GtkWidget* widget, int color){

	GdkColor newColor;
	if(color == -1) gdk_color_parse(main_color, &newColor);
	else if(color == -2) gdk_color_parse(btn_color, &newColor);
	else	gdk_color_parse(colors[color%6], &newColor);
	gtk_widget_modify_bg(widget, GTK_STATE_NORMAL, &newColor);


}

//update curr, update cars by curr state, display_screen
void show_state(GtkWidget* widget, gpointer p){

	int to = *(int*)p;
	
	if(to == NEXT) {
		curr = curr->next;
		strcpy(msg_info, "Showing next step...");
	}else if(to == PREV){
		curr = curr->prev;
		strcpy(msg_info, "Showing previous step...");
	}
	if(curr == NULL) strcpy(msg_info, "end of solution");
	else {
		cars = curr->state;
		if(update_cells(cars)) strcpy(msg_info, "error in update_cells");
	}

	gtk_label_set(GTK_LABEL(label_info), msg_info);
	gtk_widget_show_all(window);
	display_screen();
	return;
}

void exit_game(GtkWidget* widget){
	exit(EXIT_SUCCESS);
}

void reset_game(GtkWidget* widget){
	memcpy(cars, init_cars, sizeof(car_t)*n_cars);
	update_cells(cars);
	display_screen();
	strcpy(msg_info, "The game reseted.");
	gtk_label_set(GTK_LABEL(label_info), msg_info);
	gtk_widget_show_all(window);
}	


// --- v2_new


// ys_add ---

void init_node(DlistNode* head) {
   head->llink = NULL;
   head->rlink = NULL;
   head->prev = NULL;
}

void insert_Q(DlistNode* newnode) {
  newnode->rlink=NULL; 
  if(Q_tail != NULL){
      Q_tail->rlink = newnode;
   }
   Q_tail = newnode;
   if(Q_head == NULL){
      Q_head = newnode;
   }
}

DlistNode* pop() {
   DlistNode* pop_node;
   pop_node = Q_head;
   Q_head = Q_head->rlink;
   if(Q_head == NULL) Q_tail = NULL;
   return pop_node;
}

void free_all_nodes(DlistNode* head, DlistNode* tail) {
   DlistNode* p = head->rlink;
   DlistNode* next;
   while(p != NULL){
      next = p->rlink;
      free(p);
      p=next;
   }
   free(head);
   head = NULL;
   tail = NULL;
}

//Hash
int hash_function(DlistNode* node){
   int id=0;
   car_t * c = node->state;
   for(int i=0;i<n_cars;i++){
      id += (c[i].x1);
      id += (c[i].x2)*10;
      id += (c[i].y1)*100;
      id += (c[i].y2)*1000;
   }
   return id%2000;
}

int check_hash(DlistNode* node){
	int id_node = hash_function(node);
    int flag = 1;   // true
	printf("id_node: %d\n",id_node);
	printf("%p\n",hash_table[id_node]);
	if(hash_table[id_node]==NULL){
		return 0;
	}
	else{
		HashNode *temp;
		temp = hash_table[id_node];
		while(temp != NULL){
            flag = 1;
			for(int i=0;i<n_cars;i++){
				if((flag == 1)&&(temp->value->state[i].x1==node->state[i].x1)&&(temp->value->state[i].x2==node->state[i].x2)&&(temp->value->state[i].y1==node->state[i].y1)&&(temp->value->state[i].y2==node->state[i].y2)){
                    flag = 1;   // true
				} 
                else {
                    flag = 0;
                    break;
                } 
			}
            if (flag == 1)  {
                return 1;
            }
			temp = temp->next;
		}
	}
	return 0;
}

void insert_H(DlistNode* node){
   int id_node = hash_function(node);
   HashNode *newnode = (HashNode*)malloc(sizeof(HashNode));
   newnode->value = node;
   newnode->next = NULL;
   if(hash_table[id_node]==NULL){
      hash_table[id_node] = newnode;
   }
   else{
      HashNode *temp = hash_table[id_node];
      while(temp->next != NULL){
         temp = temp->next;
      }
      temp->next = newnode;
   }
}

DlistNode* make_adjnode(DlistNode* cur){
    car_t * c = cur->state;
    DlistNode * newAdj = (DlistNode*)malloc(sizeof(DlistNode));
    init_node(newAdj);
    newAdj->prev = cur;
    newAdj->state = (car_t*)malloc(sizeof(car_t)*n_cars);
    for(int j=0;j<n_cars;j++){
        newAdj->state[j].id = c[j].id;
        newAdj->state[j].x1 = c[j].x1;
        newAdj->state[j].x2 = c[j].x2;
        newAdj->state[j].y1 = c[j].y1;
        newAdj->state[j].y2 = c[j].y2;
        newAdj->state[j].span = c[j].span;
        newAdj->state[j].dir = c[j].dir;
    }
    return newAdj;
}

DlistNode* find_adj(DlistNode* cur)
{
    DlistNode* list_head = NULL;
    car_t * c = cur->state;
    for(int i=0; i<n_cars; i++){
        if(c[i].dir == horizontal){
            //check left
         if((c[i].x1-1 >= 0) && cells[c[i].y1][c[i].x1-1] == 0){
                //make new node
                DlistNode * newAdj = make_adjnode(cur);
                //move left
                newAdj->state[i].x1--;
              newAdj->state[i].x2--;
                //add to list
                newAdj->rlink = list_head;
                list_head = newAdj;

                // if(update_cells(newAdj->state)) {
                //     fprintf(stderr, "[error] duplicated cars on cells\n");
                // }
                // display();
                // if(update_cells(c)) {
                //     fprintf(stderr, "[error] duplicated cars on cells\n");
                // }
            }
            //check right
            if((c[i].x2+1 <= 5) && cells[c[i].y2][c[i].x2+1] == 0){
                //make new node
                DlistNode * newAdj = make_adjnode(cur);
                //move right
                newAdj->state[i].x1++;
              newAdj->state[i].x2++;
                //add to list
                newAdj->rlink = list_head;
                list_head = newAdj;
                // if(update_cells(newAdj->state)) {
                //     fprintf(stderr, "[error] duplicated cars on cells\n");
                // }
                // display();
                // if(update_cells(c)) {
                //     fprintf(stderr, "[error] duplicated cars on cells\n");
                // }
            }
      }
        if(c[i].dir == vertical){
            //check down
         if((c[i].y2-1 >= 0) && cells[c[i].y2-1][c[i].x2] == 0){
                //make new node
                DlistNode * newAdj = make_adjnode(cur);
                //move left
                newAdj->state[i].y1--;
              newAdj->state[i].y2--;
                //add to list
                newAdj->rlink = list_head;
                list_head = newAdj;
                // if(update_cells(newAdj->state)) {
                //     fprintf(stderr, "[error] duplicated cars on cells\n");
                // }
                // display();
                // if(update_cells(c)) {
                //     fprintf(stderr, "[error] duplicated cars on cells\n");
                // }
            }
            if((c[i].y1+1 <= 5) && cells[c[i].y1+1][c[i].x1] == 0){
                //make new node
                DlistNode * newAdj = make_adjnode(cur);
                //move left
                newAdj->state[i].y1++;
              newAdj->state[i].y2++;
                //add to list
                newAdj->rlink = list_head;
                list_head = newAdj;
                // if(update_cells(newAdj->state)) {
                //     fprintf(stderr, "[error] duplicated cars on cells\n");
                // }
                // display();
                // if(update_cells(c)) {
                //     fprintf(stderr, "[error] duplicated cars on cells\n");
                // }
            }
      }
    }

    return list_head;
}

int find_path(){
    printf("i'm find_path!!\n");

   Q_head = (DlistNode*)malloc(sizeof(DlistNode));
   init_node(Q_head);
   Q_head->state = cars;
   Q_tail = Q_head;


   while(Q_head != NULL){
      // sleep(1);
      DlistNode *s = pop();
      // if(check_hash(s)){
      //    printf("Not save in hashtable\n");
      //    continue;
      // }
      insert_H(s);

      if(update_cells(s->state)) {
         fprintf(stderr, "[error] duplicated cars on cells\n");
         return 1;
      }
        printf("s: \n");
        display();
      if(cells[3][5] == 1){
            printf("success to find solution!!\n");
         Final = s;
		 make_solution();
	 	 update_cells(cars);
         return 0;
      }
      DlistNode * Adj_head = find_adj(s);
        if(Adj_head == NULL){
            printf("There is no solution\n");
        }
      DlistNode * temp = Adj_head;
      while(temp != NULL){
         if(update_cells(temp->state)) {
                fprintf(stderr, "[error] duplicated cars on cells\n");
            }
            display();
            if(update_cells(cars)) {
                fprintf(stderr, "[error] duplicated cars on cells\n");
            }
         temp = temp->rlink;
      }

        DlistNode * next = Adj_head;
      printf("---------------------------\n");
      while(next != NULL){
         if(update_cells(next->state)) {
                fprintf(stderr, "[error] duplicated cars on cells\n");
            }
            display();
            
         if(check_hash(next)!=1){
            Adj_head = next->rlink;
			insert_Q(next);
            next=Adj_head;
         }
         else{
            printf("!!!!!!!!!!!\n");
            next=next->rlink;
         }
      }
      Adj_head = NULL;
      printf("---------------------------\n");
   }
   printf("There is no solution\n");
   return 1;
}


void make_solution(){
   DlistNode * temp;
   temp = Final;
   while(temp->prev != NULL){
      temp->prev->next = temp;
      temp = temp->prev;
   }
   Solution = temp;
}

void show_solution(){
   DlistNode * temp;
   temp = Solution;
   while(temp != NULL){
      if(update_cells(temp->state)) {
         fprintf(stderr, "[error] duplicated cars on cells\n");
      }
      display();
      temp = temp->next;
   }
}





// --- ys_add


