#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <gtk/gtk.h>
#define NUM_PLAYER 4
#define CELL_SIZE 8
#define MAP_WIDTH 32
#define MAP_LENGTH 16
#define SCORE_BOARD_WIDTH 64
#define INFO_LABEL_LENGTH 16
#define BUF_SIZE 128

int map[MAP_WIDTH][MAP_LENGTH]; //-1: empty, -2: block, -3 fruit, 0 >= users
char msg_info[BUF_SIZE] = "";
char username[BUF_SIZE] = "me";
char buf[BUF_SIZE] = "";
char score[NUM_PLAYER][BUF_SIZE];
void load_game_info();
enum entity = {
	EMPTY = 0,
	ITEM = -1,
	BLOCK = -2,
	USER1 = 1,
	USER2 = 2,
	USER3 = 3,
	USER4 = 4,
	BASE1 = 10,
	BASE2 = 20,
	BASE3 = 30,
	BASE4 = 40
};


//for GUI
GtkWidget *window;
GtkWidget *mat_main, *mat_screen, *mat_board, *label_info, *label_me;
GtkWidget *mat_ans_btn, *mat_sol_btn;
GtkWidget *btn_solve, *btn_exit, *btn_next, *btn_prev;
GtkWidget *btn_auto, *btn_up, *btn_down, *btn_left, *btn_right;
GtkWidget *label_name, *label_score[NUM_PLAYER]; 
GdkPixbuf *icon, *icon_block[2], *icon_fruit[10], *icon_player[NUM_PLAYER]; 
GdkPixbuf *create_pixbuf(const gchar * filename);
int load_icons();
int check_map_valid();
void set_window();
GtkWidget *create_base(int id);
void display_screen();
void add_mat_board();
void exit_game(GtkWidget* widget);
char user_color[id][20] = {}

//for testing...
void test_set(){
	g_print(" ... test ... \n");
	for(int i = 0; i < NUM_PLAYER; i++){
		sprintf(score[i], "user%d: %d", i, i+1);		
		g_print("score%d -> %s\n", i, score[i]);
	}
	
	for(int i = 0; i < MAP_WIDTH, i++){
		for(int j = 0; j < MAP_LENGTH; j++){
			map[i][j] = EMPTY;
		}
	}
	
	srand((unsigned int)time(NULL));
    int randx, randy;
	for (int i = 0; i < 50; i++) {
        randx = rand() % MAP_WIDTH+1;
        randy = rand() % MAP_LENGTH+1;
		map[ranx][randy] = BLOCK;
    }
	for (int i = 0; i < 20; i++) {
        randx = rand() % MAP_WIDTH+1;
        randy = rand() % MAP_LENGTH+1;
		map[ranx][randy] = ITEM;
    }


}

int main(int argc, char *argv[]) {

	//get the username from stdin 
	//TODO maybe need change to args
	while(1){
		printf("enter your name: ");
		if((scanf("%s", username) != 1) || 0 /*TODO need another checking?*/){
			printf("invalid name. please pick another one.");	
			continue;
		}else break;
	}

	//load icons from icons dir
	if(load_icons()) {
		g_print("failed to load icons\n");
		return -1;
  	}

	//set the testing data 
	// TODO can be customize
	test_set();

	//set the GUI
	gtk_init(&argc, &argv); //init GTK by args
	set_window();

 	return 0;
}


//get pixbuf(for load image) from filename
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


//load icons needed
//0 on success, 1 on failure
int load_icons(){
   	GdkPixbuf *pixbuf;
	for(int i = 0; i < NUM_PLAYER; i++){
		sprintf(buf, "icons/user%d.png", i);
		if((pixbuf = create_pixbuf(buf)) == NULL) return 1;
		else g_print("loading %s...\n", buf);
		icon_player[i] = gdk_pixbuf_scale_simple(pixbuf, 32, 32, GDK_INTERP_BILINEAR);
		g_object_unref(pixbuf);
	}
	for(int i = 0; i < 2; i++){
		sprintf(buf, "icons/block%d.png", i);
		if((pixbuf = create_pixbuf(buf)) == NULL) return 1;
		else g_print("loading %s...\n", buf);
		icon_block[i] = gdk_pixbuf_scale_simple(pixbuf, 32, 32, GDK_INTERP_BILINEAR);
		g_object_unref(pixbuf);
	}
	for(int i = 0; i < 12; i++){
		sprintf(buf, "icons/fruit%d.png", i);
		if((pixbuf = create_pixbuf(buf)) == NULL) return 1;
		else g_print("loading %s...\n", buf);
		icon_fruit[i] = gdk_pixbuf_scale_simple(pixbuf, 32, 32, GDK_INTERP_BILINEAR);
		g_object_unref(pixbuf);
	}

	g_print("success to load all icons!\n");
	return 0;

}


//load game information from server and save map[]
//TODO replace test to this one when done
void load_game_info(){





}


//GUI: set the main window
void set_window(){

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);//make window
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);//for termination

  //set the window
  gtk_window_set_title(GTK_WINDOW(window), "pushpush HK");
//  gtk_window_set_default_size(GTK_WINDOW(window), (MAP_WIDTH*CELL_SIZE)+SCORE_BOARD_WIDTH, (MAP_LENGTH*CELL_SIZE)+(INFO_LABEL_LENGTH*2));
  gtk_window_set_default_size(GTK_WINDOW(window), 1024, 512);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(window), 5);

  //change the icon of page(for cuteness)
  icon = create_pixbuf("icons/catIcon.png");
  gtk_window_set_icon(GTK_WINDOW(window), icon);

  
  //set main matrix
  mat_main = gtk_table_new(10, 10, TRUE);
  gtk_container_add(GTK_CONTAINER(window), mat_main);
	
  strcpy(msg_info, "Welcome to PushPush HK!");
  label_info = gtk_label_new(msg_info);
  gtk_table_attach_defaults(GTK_TABLE(mat_main), label_info, 0, 11, 0, 1);
  
  display_screen();
  add_mat_board();
 
  label_me = gtk_label_new("23-winter capston study#2 leeejjju");
  gtk_misc_set_alignment(GTK_MISC(label_me), 1.0, 1.0);
  gtk_table_attach_defaults(GTK_TABLE(mat_main), label_me, 0, 11, 10, 11);

  gtk_widget_show_all(window); //is it dup with above
  g_object_unref(icon);
  gtk_main(); //enter the GTK main loop

}



GtkWidget *create_base(int id){

	GtkWidget* base = gtk_event_box_new();
	g_signal_connect(G_OBJECT(sprite), "expose-event", G_CALLBACK(draw_base), &id);
	gtk_widget_set_size_request(sprite, 32, 32);		

	GdkColor color;
	gdk_color_parse(user_color[id], color);


}

//GUI: display screen from map[] model
//TODO need to be updated to display by map[]
void display_screen(){

  //set screen matrix
  mat_screen = gtk_table_new(MAP_WIDTH, MAP_LENGTH, FALSE);
  for (int i = 0; i <= MAP_WIDTH; i++) {
    for (int j = 0; j <= MAP_LENGTH; j++) {
		int rand_index;
		GtkWidget* sprite; 
		if (map[i][j] == EMPTY) continue;
		switch(map[i][j]){
			case BLOCK:
				rand_index= rand() % 2;
      			sprite = gtk_image_new_from_pixbuf(icon_block[rand_index]); 
				break;	
			case ITEM:
				rand_index= rand() % 12;
      			sprite = gtk_image_new_from_pixbuf(icon_fruit[rand_index]); 
				break;	
			default: //this case, user or base
				if(map >= 10){ //base
					int id = map[i][j]/10-1;
					sprite = create_base(id);
				}else{ //user
					rand_index = map[i][j]-1;
      				sprite = gtk_image_new_from_pixbuf(icon_player[rand_index]); 
				}
				break;

		}
		//TODO select random index with corresponding entity set
		gtk_table_attach_defaults(GTK_TABLE(mat_screen), sprite, i, i+1, j, j+1);
    }
  }
  gtk_table_attach_defaults(GTK_TABLE(mat_main), mat_screen, 0, 9, 1, 10);

}

//GUI: set the score board
//TODO need to be updated from score received from server
void add_mat_board(){

  //set board vbox
  mat_board = gtk_vbox_new(FALSE, 20);
 
  GtkWidget* line1 = gtk_hseparator_new();
  sprintf(buf, "Good luck, %s!", username);
  label_name = gtk_label_new(buf);
  GtkWidget* sprite = gtk_image_new_from_pixbuf(icon_player[0]);
  GtkWidget* line2 = gtk_hseparator_new();
  GtkWidget *label_title = gtk_label_new(":: SCORE ::");
  
  gtk_container_add(GTK_CONTAINER(mat_board), line1);
  gtk_container_add(GTK_CONTAINER(mat_board), label_name);
  gtk_container_add(GTK_CONTAINER(mat_board), sprite);
  gtk_container_add(GTK_CONTAINER(mat_board), line2);
  gtk_container_add(GTK_CONTAINER(mat_board), label_title);

  for(int i = 0; i < NUM_PLAYER; i++){
	//TODO this part display the score (global array)
	label_score[i] = gtk_label_new(score[i]);
	gtk_container_add(GTK_CONTAINER(mat_board), label_score[i]);
  } 
 
  btn_exit = gtk_button_new_with_label("exit game");
  gtk_container_add(GTK_CONTAINER(mat_board), btn_exit);
  g_signal_connect(G_OBJECT(btn_exit), "clicked", G_CALLBACK(exit_game), NULL);
  gtk_table_attach_defaults(GTK_TABLE(mat_main), mat_board, 9, 11, 1, 10);


}

 

void exit_game(GtkWidget* widget){
	printf("See you again!\n");
	exit(EXIT_SUCCESS);
}


gboolean draw_base(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    
	int id = *(int*)data;
	cairo_t *cr = gdk_cairo_create(widget->window);

    cairo_set_source_rgb(cr, 1.0, 0.75, 0.8);

    // Draw a rectangle
    cairo_rectangle(cr, 0, 0, widget->allocation.width, widget->allocation.height);
    cairo_fill(cr);

    cairo_destroy(cr);

    return FALSE;
}



