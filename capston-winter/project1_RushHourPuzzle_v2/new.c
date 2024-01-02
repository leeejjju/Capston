#include <gtk/gtk.h>

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

int cars[6][6] = {};
char msg_info[128] = "";




void display_screen(GtkWidget* window, GtkWidget* mat_main, GtkWidget*  mat_screen){

  //set screen matrix
  mat_screen = gtk_table_new(6, 6, TRUE);
  gtk_table_set_row_spacings(GTK_TABLE(mat_screen), 2);
  gtk_table_set_col_spacings(GTK_TABLE(mat_screen), 2);
  GtkWidget *btn_tmp;
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
	  if(cars[i][j] == 0) continue;
      gchar label_text[2];
	  g_snprintf(label_text, sizeof(label_text), "%d", cars[i][j]);
      btn_tmp = gtk_button_new_with_label(label_text);
      gtk_table_attach_defaults(GTK_TABLE(mat_screen), btn_tmp, i, i+1, j, j+1);
    }
  }
  gtk_container_add(GTK_CONTAINER(window), mat_screen);
  gtk_box_pack_start(GTK_BOX(mat_main), mat_screen, TRUE, TRUE, 0);

}


void add_mat_solve(GtkWidget* window, GtkWidget* mat_main, GtkWidget*  mat_solve, GtkWidget*  label_info, GtkWidget*  btn_answer, GtkWidget*  btn_up, GtkWidget*  btn_down,  GtkWidget* btn_left, GtkWidget* btn_right, GtkWidget*  label_me){

  mat_solve = gtk_table_new(6, 4, TRUE);
  gtk_container_add(GTK_CONTAINER(window), mat_solve);
  gtk_table_set_row_spacings(GTK_TABLE(mat_solve), 5);
  gtk_table_set_col_spacings(GTK_TABLE(mat_solve), 5);

  label_info = gtk_label_new(msg_info);
  gtk_container_add(GTK_CONTAINER(window), label_info);
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), label_info, 0, 4, 0, 1);

  btn_up = gtk_button_new_with_label("up");
  btn_down = gtk_button_new_with_label("down");
  btn_left = gtk_button_new_with_label("left");
  btn_right = gtk_button_new_with_label("right");
  gtk_container_add(GTK_CONTAINER(window), btn_up);
  gtk_container_add(GTK_CONTAINER(window), btn_down);
  gtk_container_add(GTK_CONTAINER(window), btn_left);
  gtk_container_add(GTK_CONTAINER(window), btn_right);
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), btn_up, 1, 3, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), btn_down, 1, 3, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), btn_left, 0, 2, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), btn_right, 2, 4, 2, 3);
  
  btn_answer = gtk_button_new_with_label("show answer");
  gtk_container_add(GTK_CONTAINER(window), btn_answer);
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), btn_answer, 0, 4, 4, 5); 
 
  label_me = gtk_label_new("23-winter capston study#1 leeejjju");
  gtk_container_add(GTK_CONTAINER(window), label_me);
  gtk_misc_set_alignment(GTK_MISC(label_me), 1.0, 1.0);
  gtk_table_attach_defaults(GTK_TABLE(mat_solve), label_me, 2, 4, 5, 6);

  gtk_box_pack_start(GTK_BOX(mat_main), mat_solve, TRUE, TRUE, 0);


} 

void add_mat_answer(GtkWidget* window, GtkWidget* mat_main, GtkWidget*  mat_answer, GtkWidget*  label_info, GtkWidget*  btn_solve, GtkWidget*  btn_next, GtkWidget*  btn_prev,GtkWidget*  label_me){
 
  mat_answer = gtk_table_new(6, 4, TRUE);
  gtk_container_add(GTK_CONTAINER(window), mat_answer);
  gtk_table_set_row_spacings(GTK_TABLE(mat_answer), 5);
  gtk_table_set_col_spacings(GTK_TABLE(mat_answer), 5);

  label_info = gtk_label_new(msg_info);
  gtk_container_add(GTK_CONTAINER(window), label_info);
  gtk_table_attach_defaults(GTK_TABLE(mat_answer), label_info, 0, 4, 0, 1);

  btn_prev = gtk_button_new_with_label("prev step");
  btn_next = gtk_button_new_with_label("next step");
  gtk_container_add(GTK_CONTAINER(window), btn_prev);
  gtk_container_add(GTK_CONTAINER(window), btn_next);
  gtk_table_attach_defaults(GTK_TABLE(mat_answer), btn_prev, 0, 2, 1, 4);
  gtk_table_attach_defaults(GTK_TABLE(mat_answer), btn_next, 2, 4, 1, 4);
  
  btn_solve = gtk_button_new_with_label("show answer");
  gtk_container_add(GTK_CONTAINER(window), btn_solve);
  gtk_table_attach_defaults(GTK_TABLE(mat_answer), btn_solve, 0, 4, 4, 5); 
 
  label_me = gtk_label_new("23-winter capston study#1 leeejjju");
  gtk_container_add(GTK_CONTAINER(window), label_me);
  gtk_misc_set_alignment(GTK_MISC(label_me), 1.0, 1.0);
  gtk_table_attach_defaults(GTK_TABLE(mat_answer), label_me, 2, 4, 5, 6);

  //TODO modify color

  gtk_box_pack_start(GTK_BOX(mat_main), mat_answer, TRUE, TRUE, 0);

} 

int main(int argc, char *argv[]) {

  for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 6; j++) {
        cars[i][j] = 0;
	  }
  }

  cars[2][0] = 2;
  cars[2][1] = 2;
  cars[2][2] = 2;
  cars[0][2] = 1;
  cars[1][2] = 1;
  cars[0][3] = 3;
  cars[1][3] = 3;
  cars[2][3] = 3;
  cars[5][3] = 4;
  cars[5][4] = 4;
  cars[5][5] = 4;

  gtk_init(&argc, &argv); //init GTK by args
  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);//make window

  //set the window
  gtk_window_set_title(GTK_WINDOW(window), "rushhour v2");
  gtk_window_set_default_size(GTK_WINDOW(window), 480, 640);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(window), 5);

  //change the icon of page(for cuteness)
  GdkPixbuf *icon = create_pixbuf("catIcon.png");
  gtk_window_set_icon(GTK_WINDOW(window), icon);

  //add widgets...
  GtkWidget *mat_main, *mat_solve, *mat_answer;
  GtkWidget *mat_screen, *mat_ans_btn, *mat_sol_btn;
  GtkWidget *btn_solve, *btn_answer, *btn_next, *btn_prev;
  GtkWidget *btn_auto, *btn_up, *btn_down, *btn_left, *btn_right;
  GtkWidget *label_info, *label_me;

  //set main matrix
  mat_main = gtk_vbox_new(FALSE, 20);
  gtk_container_add(GTK_CONTAINER(window), mat_main);


  display_screen(window, mat_main, mat_screen);
//  add_mat_solve(window, mat_main, mat_solve, label_info, btn_answer, btn_up, btn_down, btn_left, btn_right, label_me);
  add_mat_answer(window, mat_main, mat_answer, label_info, btn_solve, btn_next, btn_prev, label_me); 


  gtk_widget_show_all(window); //is it dup with above
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);//for termination
  g_object_unref(icon);
  gtk_main(); //enter the GTK main loop



  return 0;





}
