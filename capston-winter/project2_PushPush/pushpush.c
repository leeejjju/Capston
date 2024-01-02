#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#define NUM_PLAYER 4
#define CELL_SIZE 8
#define MAP_WIDTH 32
#define MAP_LENGTH 16
#define SCORE_BOARD_WIDTH 64
#define INFO_LABEL_LENGTH 16
#define BUF_SIZE 128

int map[6][6] = {};
char msg_info[BUF_SIZE] = "";
char username[BUF_SIZE] = "me";
GtkWidget *window;
GtkWidget *mat_main, *mat_screen, *mat_board, *label_info, *label_me;
GtkWidget *mat_ans_btn, *mat_sol_btn;
GtkWidget *btn_solve, *btn_exit, *btn_next, *btn_prev;
GtkWidget *btn_auto, *btn_up, *btn_down, *btn_left, *btn_right;
GtkWidget *label_name, *label_score[NUM_PLAYER]; 
GdkPixbuf *icon;
char score[NUM_PLAYER][BUF_SIZE];
char buf[BUF_SIZE] = "";

GdkPixbuf *create_pixbuf(const gchar * filename);
void load_sprites();
void set_window();
void display_screen();
void add_mat_board();

void test_set(){
	for(int i = 0; i < NUM_PLAYER; i++){
		sprintf(score[i], "user%d: %d", i, i+1);		
	}
}


int main(int argc, char *argv[]) {

	//TODO get username from stdin

	while(1){
		printf("enter your name: ");
		if((scanf("%s", username) != 1) || 0 /*TODO need another checking?*/){
			printf("invalid name. please pick another one.");	
			continue;
		}else break;
	}

  gtk_init(&argc, &argv); //init GTK by args
  
  test_set();

  set_window();

  return 0;
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
  icon = create_pixbuf("catIcon.png");
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


void display_screen(){

  //set screen matrix
  mat_screen = gtk_table_new(MAP_WIDTH, MAP_LENGTH, FALSE);
  GtkWidget *btn_tmp;
  for (int i = 0; i <= MAP_WIDTH; i++) {
    for (int j = 0; j <= MAP_LENGTH; j++) {
      btn_tmp = gtk_button_new_with_label("");
      gtk_table_attach_defaults(GTK_TABLE(mat_screen), btn_tmp, i, i+1, j, j+1);
    }
  }
  gtk_table_attach_defaults(GTK_TABLE(mat_main), mat_screen, 0, 9, 1, 10);

}

void add_mat_board(){

  //set board vbox
  mat_board = gtk_vbox_new(FALSE, 20);
 
  GtkWidget *label_title;
  label_title = gtk_label_new(":: Push Push ::");
  GtkWidget* line1 = gtk_hseparator_new();
  sprintf(buf, "Good luck, %s!", username);
  label_name = gtk_label_new(buf);
  GtkWidget* line2 = gtk_hseparator_new();
  
  gtk_container_add(GTK_CONTAINER(mat_board), label_title);
  gtk_container_add(GTK_CONTAINER(mat_board), line1);
  gtk_container_add(GTK_CONTAINER(mat_board), label_name);
  gtk_container_add(GTK_CONTAINER(mat_board), line2);

  for(int i = 0; i < NUM_PLAYER; i++){
		g_print("score%d -> %s\n", i, score[i]);
	label_score[i] = gtk_label_new(score[i]);
	gtk_container_add(GTK_CONTAINER(mat_board), label_score[i]);
  } 
 
  btn_exit = gtk_button_new_with_label("exit game");
  gtk_container_add(GTK_CONTAINER(mat_board), btn_exit);

  gtk_table_attach_defaults(GTK_TABLE(mat_main), mat_board, 9, 11, 1, 10);


} 
