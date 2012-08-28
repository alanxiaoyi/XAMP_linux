/**************license begin*******************

Copyright (c) 2012, Yan Solihin and Yipeng Wang,
affiliated with the Department of Electrical and Computer Engineering at North Carolina State University.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


***************license end*********************/

#include <gtk/gtk.h>
#include "interface.h"

extern int find_model_arc(list<string> *input_list, string output);			//from model_arc
extern list<model_class> allcandidate[LENGTH];								//from model_arc

list<string> input_name_list;
string output_name;

using namespace std;
typedef struct{
GtkWidget *text;
GtkWidget *iotext;
GtkWidget *desctext;
GtkWidget	*editor;
}textstruct;

GtkWidget *main_window, *result_text, *second_window; 
GtkTreeSelection *selection_in, *selection_out;
GtkWidget *list_text_0, *list_text_1, *list_text_2, *list_text_3; 


int combo_count=0;
list<model_class>::iterator themodel;
GThread *thread;
/*processing_dialog
*/

void close_processing_dialog( GtkWidget *widget){
	g_print("You stop the process\n"); 
	exit(0);
}



static gpointer thread_func(gpointer Gpointer){

	gdk_threads_enter();

	GtkWidget *processing_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window),
								 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_QUESTION,
                                 GTK_BUTTONS_CLOSE,
                                 "Processing...model is processing\nClick cancel if no response");	
	g_signal_connect(processing_dialog,"response", G_CALLBACK(close_processing_dialog),NULL);
	gtk_widget_show_all (processing_dialog);	
	gdk_threads_leave();	
	string result="";
	char tmp[256];
	char cmd_char[256];
	string cmd;
	g_print("push enter_button\n"); 
	strcpy(cmd_char,gtk_entry_get_text(GTK_ENTRY(Gpointer)));
	cmd=string(cmd_char);
	init_model(themodel , 0, cmd );	
	ifstream infile("pipe.tmp");	
	if(!infile) {cout<<"open pipe.tmp fail(no output to stdio)"<<endl; return NULL;}	
	while(infile.getline(tmp,256)){
		result=result+"\n"+string(tmp)	;
	}	
	gdk_threads_enter();
	gtk_label_set_text(GTK_LABEL(result_text),result.c_str());	
	gtk_widget_destroy(processing_dialog);
	gdk_threads_leave();

	remove("pipe.tmp"); 						//delete the tmp file;

	g_thread_exit(NULL);
	return NULL;
}




/*create icon
*/
GdkPixbuf *create(const gchar * filename)
{
   GdkPixbuf *pixbuf;
   GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);
   if(!pixbuf) {
      fprintf(stderr, "%sn", error->message);
      g_error_free(error);
   }
   return pixbuf;
}


/*Exit button
*/
void cb_qbutton(GtkWidget *widget, gpointer data) {
	g_print("push exit_button\n"); 
	gtk_main_quit(); 
} 


/*add to list
*/
void add_to_list(GtkWidget *list, const gchar *str)
{
  GtkListStore *store;
  GtkTreeIter iter;

  store = GTK_LIST_STORE(gtk_tree_view_get_model
      (GTK_TREE_VIEW(list)));

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, 0, str, -1);
}

/*each input selected
*/
void  view_selected_in_foreach_func (GtkTreeModel  *model,
                              GtkTreePath   *path,
                              GtkTreeIter   *iter,
                              gpointer       userdata){
    char *name;

    gtk_tree_model_get (model, iter, 0, &name, -1);
	string name_s=string(name);
	input_name_list.push_back(name_s);
    printf ("%s is selected\n", name);
}

/*each output selected
*/
void   view_selected_out_foreach_func (GtkTreeModel  *model,
                              GtkTreePath   *path,
                              GtkTreeIter   *iter,
                              gpointer       userdata){
    char *name;

    gtk_tree_model_get (model, iter, 0, &name, -1);
 	string name_s=string(name);
	output_name=name_s;
    printf ("%s is selected\n", name);
}


/*search arc button
*/
void cb_search_button(GtkWidget *widget, gpointer data) {
	string r[4];
	 input_name_list.clear();
	 output_name="";
	 gtk_tree_selection_selected_foreach(selection_in, view_selected_in_foreach_func, NULL);	
	 gtk_tree_selection_selected_foreach(selection_out, view_selected_out_foreach_func, NULL);
	 

	
	for(int i=0; i<4; i++){
		allcandidate[i].clear();
	}
	
	if(!input_name_list.empty() && output_name!="");
	int a = find_model_arc(&input_name_list, output_name);		
	
	list<model_class>::iterator it;
	for(int i=0; i<LENGTH; i++){
		if(!allcandidate[i].empty()){
			for(it=allcandidate[i].begin(); it!=allcandidate[i].end(); it++){				
				r[i]=r[i]+it->name+"\n";	
			}			
		}	
	}
	cout<<"hahaha"<<r[0]<<endl;
	gtk_label_set_text(GTK_LABEL(list_text_0),r[0].c_str());
	gtk_label_set_text(GTK_LABEL(list_text_1),r[1].c_str());
	gtk_label_set_text(GTK_LABEL(list_text_2),r[2].c_str());
	gtk_label_set_text(GTK_LABEL(list_text_3),r[3].c_str());	

	
}



/*arc button
*/
void cb_abutton(GtkWidget *widget, gpointer data) {
	g_print("push arc_button\n"); 
	GtkWidget *vbox, *hbox,*hbox_result,*arrow_frame[3], *searching_arc_button;
	GtkWidget *scrolledwindow_in,*scrolledwindow_out;
	GtkWidget *arrow[3];
	
	for(int i=0; i<3; i++){		
		arrow_frame[i]=gtk_frame_new ("");	
		arrow[i]=gtk_arrow_new(GTK_ARROW_RIGHT,GTK_SHADOW_OUT);	
	}
	
	list_text_0=gtk_label_new("");	
	list_text_1=gtk_label_new("");	
	list_text_2=gtk_label_new("");	
	list_text_3=gtk_label_new("");		
	gtk_label_set_line_wrap( GTK_LABEL( list_text_0), TRUE );
	gtk_widget_set_size_request(  list_text_0 , 200, 200 );
	gtk_label_set_line_wrap( GTK_LABEL( list_text_1), TRUE );
	gtk_widget_set_size_request(  list_text_1 , 200, 200 );
	gtk_label_set_line_wrap( GTK_LABEL( list_text_2), TRUE );
	gtk_widget_set_size_request(  list_text_2 , 200, 200 );
	gtk_label_set_line_wrap( GTK_LABEL( list_text_3), TRUE );
	gtk_widget_set_size_request(  list_text_3 , 200, 200 );

	
	
	scrolledwindow_in = gtk_scrolled_window_new(NULL,NULL);
	scrolledwindow_out = gtk_scrolled_window_new(NULL,NULL);

	gtk_widget_set_size_request(scrolledwindow_in, 430, 170);		//guide
	gtk_widget_set_size_request(scrolledwindow_out, 430, 170);		//iotext	
	
	
	
	vbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);

	
	hbox_result = gtk_hbox_new(FALSE, 0);		
	list<string> output_list;
	
	searching_arc_button = gtk_button_new_with_label("Searching");	
	g_signal_connect(G_OBJECT(searching_arc_button), "clicked", G_CALLBACK(cb_search_button),NULL);
	
	list<model_class>::iterator itm;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column_in, *column_out;
	GtkListStore *store_in, *store_out;
	GtkWidget *list_in, *list_out;
	renderer = gtk_cell_renderer_text_new();
	
	column_in = gtk_tree_view_column_new_with_attributes("input list", renderer, "text", 0, NULL);
	column_out = gtk_tree_view_column_new_with_attributes("output list", renderer, "text", 0, NULL);
	
	list_in = gtk_tree_view_new();
	list_out = gtk_tree_view_new();
	store_in = gtk_list_store_new(1, G_TYPE_STRING);  
	store_out = gtk_list_store_new(1, G_TYPE_STRING);  	
	
	gtk_tree_view_append_column(GTK_TREE_VIEW(list_in), column_in);
	gtk_tree_view_append_column(GTK_TREE_VIEW(list_out), column_out);
 
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(list_in), GTK_TREE_MODEL(store_in));
	gtk_tree_view_set_model(GTK_TREE_VIEW(list_out), GTK_TREE_MODEL(store_out));
	selection_in = gtk_tree_view_get_selection(GTK_TREE_VIEW(list_in));
	selection_out = gtk_tree_view_get_selection(GTK_TREE_VIEW(list_out));

	gtk_tree_selection_set_mode (selection_in, GTK_SELECTION_MULTIPLE);
	
	second_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(second_window), "seeking model arc");	
	gtk_window_set_default_size(GTK_WINDOW(second_window), 900,700); 	
	gtk_window_set_policy (GTK_WINDOW(second_window), true, true, true);
	
	for(itm=model_list.begin(); itm!=model_list.end();itm++){
		int n=0;
		while(itm->output[n][0]!=""){
			output_list.push_back(itm->output[n][0]);
			n++;
		}
	}
	output_list.sort();
	output_list.unique();
	

	list<input_class>::iterator icit;
	list<string>::iterator sit;
	for(icit=input_list.begin();icit!=input_list.end();icit++){		
		add_to_list(list_in, (icit->name).c_str());
	}
	for(sit=output_list.begin();sit!=output_list.end();sit++){		
		add_to_list(list_out, (*sit).c_str());
	}
		
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow_in),list_in);
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow_out),list_out);
	for(int i=0; i<3; i++){
		gtk_container_add (GTK_CONTAINER (arrow_frame[i]), arrow[i]);
	}	
	gtk_box_pack_start(GTK_BOX(hbox), scrolledwindow_in, TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(hbox), scrolledwindow_out, TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(hbox_result), list_text_0, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox_result), arrow_frame[0], TRUE, TRUE, 5);		
	gtk_box_pack_start(GTK_BOX(hbox_result), list_text_1, TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(hbox_result), arrow_frame[1], TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(hbox_result), list_text_2, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox_result), arrow_frame[2], TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(hbox_result), list_text_3, TRUE, TRUE, 5);
	
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(vbox), searching_arc_button, TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(vbox), hbox_result, TRUE, TRUE, 5);	
	gtk_container_add(GTK_CONTAINER(second_window), vbox);	
	
	int x;
	int y;
	gtk_window_get_position(GTK_WINDOW(main_window),&x,&y);
	
	gtk_widget_show_all(second_window);	
	gtk_window_move(GTK_WINDOW(second_window),x,y+50);
} 

/*enter button
*/
void cb_ebutton(GtkWidget *widget,GtkEntry *editor) {
	
//try to fork new process to run the model since we don't want it block the window
			
	thread=NULL;
	GError* error=NULL;
	thread = g_thread_create( thread_func, (gpointer)editor,
                              FALSE, &error );

	if (!thread){
		cout<<"call model as child thread fail!"<<endl;
	}
} 

/*Error dialog
*/
void error_loading_dialog(){
	GtkWidget *error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window),
								 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_CLOSE,
                                 "ERROR loading XML file");	
	gtk_widget_show(error_dialog);
	g_signal_connect_swapped (error_dialog, "response",
								G_CALLBACK (gtk_widget_destroy),
								error_dialog);								 
}	

/*parse input button
*/
void cb_ibutton(GtkWidget *widget, GtkWidget * combo) {
				list<model_class>::iterator itm;
				g_print("push parse input button\n"); 
				parse_input_file(input_file_name);
			if(combo_count!=0){
				for(int i=0; i<combo_count; i++)					
					gtk_combo_box_remove_text (GTK_COMBO_BOX(combo), 0);
			}
			combo_count=0;
			for(itm=model_list.begin(); itm!=model_list.end();itm++){						//init combo box
				combo_count++;
				stringstream ss;
				ss<<itm->num;
				if(itm->ready==0)
					gtk_combo_box_append_text(GTK_COMBO_BOX(combo), ((ss.str())+". "+(itm->name)).c_str());
				else gtk_combo_box_append_text(GTK_COMBO_BOX(combo), ("*"+(ss.str())+". "+(itm->name)).c_str());
			}				
} 

/* input deduction button
*/
void cb_dbutton(GtkWidget *widget, gpointer data) {
				g_print("push input deduction button\n"); 
				input_deduction();

} 


/*parse config file button
*/
void cb_cbutton(GtkWidget *widget, GtkWidget * combo) {
				list<model_class>::iterator itm;
				g_print("push config button\n"); 
				GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(main_window),
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_QUESTION,
                                 GTK_BUTTONS_YES_NO,
                                 "Do you want to create a new input file?");
				gint result = gtk_dialog_run (GTK_DIALOG (dialog));				//get the user's choice
				gtk_widget_destroy (dialog);

				if(result==-8) {
					if(!parse_config(config_file_name,true/*create new input file or not*/))
						error_loading_dialog();		
					parse_input_file(input_file_name);
					
				}
				else {
					if(!parse_config(config_file_name,false/*create new input file or not*/))	
						error_loading_dialog()	;	
					parse_input_file(input_file_name);						
				}
			if(combo_count!=0){
				for(int i=0; i<combo_count; i++)					
					gtk_combo_box_remove_text (GTK_COMBO_BOX(combo), 0);
			}
			combo_count=0;
			for(itm=model_list.begin(); itm!=model_list.end();itm++){						//init combo box
				combo_count++;
				stringstream ss;
				ss<<itm->num;
				if(itm->ready==0)
					gtk_combo_box_append_text(GTK_COMBO_BOX(combo), ((ss.str())+". "+(itm->name)).c_str());
				else gtk_combo_box_append_text(GTK_COMBO_BOX(combo), ("*"+(ss.str())+". "+(itm->name)).c_str());
			}
} 

/*combo menu choose
*/
void cb_menu(GtkComboBox *combo, textstruct* data){
	list<model_class>::iterator itm;

	GtkWidget *otherwgt1=(GtkWidget*) (data->text);
	GtkWidget *otherwgt2=(GtkWidget*) (data->iotext);
	GtkWidget *otherwgt3=(GtkWidget*) (data->desctext);
	GtkWidget *otherwgt4=(GtkWidget*) (data->editor);
	int a=gtk_combo_box_get_active (combo);
	string tmp;
	for(itm=model_list.begin(); itm!=model_list.end();itm++){
		if(itm->num==a+1) break;
	}
	if(itm!=model_list.end()){
		themodel=itm;
		int n=0;
		tmp="input: \n\n";
		while(itm->input[n][0]!=""){
			tmp=tmp+itm->input[n][0]+": "+itm->input[n][1]+"\t\t//"+itm->input[n][2]+"\n";
			n++;			
		}
		gtk_entry_set_text (GTK_ENTRY(otherwgt4), (itm->dft[0]).c_str());
		gtk_label_set_text(GTK_LABEL(otherwgt2),tmp.c_str());
		gtk_label_set_text(GTK_LABEL(otherwgt1), ("User Guide for The Model:\n\n"+itm->guide).c_str());
		gtk_label_set_text(GTK_LABEL(otherwgt3), ("Description for The Model:\n\n"+itm->comment).c_str());

	}
}

int call_gui() { 
	g_thread_init(NULL);
    gdk_threads_init();
	gtk_init(NULL, NULL);
	GtkWidget *hbox_head,*hbox1, *hbox2, *hbox3 ,*hbox4,*hbox4_1,*hbox4_2,*hbox_desc, *vbox;
	GtkWidget *arc_button, *exit_button,*input_button,*deduction_button,*enter_button,*config_button,  *combo,*combotext,*headtext;
	GtkWidget *buttonbox1,*frame0, *frame1, *frame2, *frame3, *frame4, *frame5, *vbox_s3,*vbox_s2;
	GtkWidget *scrolledwindow1,*scrolledwindow2,*scrolledwindow3,*scrolledwindow4,*scrolledwindow_all;
	GtkObject *vadjust,*hadjust;
	textstruct *textmove=g_slice_new(textstruct);

		 
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(main_window), "destroy", G_CALLBACK(gtk_main_quit), NULL); 
	gtk_window_set_title(GTK_WINDOW(main_window), "XAMP"); 
	gtk_window_set_default_size(GTK_WINDOW(main_window), 930,830); 
	gtk_window_set_policy (GTK_WINDOW(main_window), true, true, true);
	gtk_window_set_icon(GTK_WINDOW(main_window), create("icon.png"));

	
	
	/*combo box
	*/
	combo=gtk_combo_box_new_text ();
	gtk_combo_box_set_add_tearoffs (GTK_COMBO_BOX(combo),true);
	gtk_combo_box_set_title (GTK_COMBO_BOX(combo),"choose a model");

	/*label
	*/
	textmove->text=gtk_label_new("");
	textmove->iotext=gtk_label_new("");
	textmove->desctext=gtk_label_new("");
	result_text=gtk_label_new("");
	gtk_label_set_selectable (GTK_LABEL(result_text), true);
	headtext=gtk_label_new("This is the GUI model platform. Please use it as 3 steps as below, and also refer to README for more information. Copyright (c)2012, Yan Solihin and Yipeng Wang. All Rights Reserved.");
	gtk_widget_set_size_request(  headtext , 700, -1 );
	gtk_label_set_line_wrap (GTK_LABEL(headtext), true);	
	combotext=gtk_label_new("Choose model:");
	gtk_label_set_line_wrap( GTK_LABEL( result_text), TRUE );
	gtk_widget_set_size_request(  result_text , 700, -1 );
	gtk_label_set_line_wrap (GTK_LABEL(textmove->text), true);			//guide text wrap
	gtk_widget_set_size_request(textmove->text, 370,-1);				//wrap on the bound of the scrolled window
	gtk_label_set_line_wrap (GTK_LABEL(textmove->desctext), true);		//description text wrap
	gtk_widget_set_size_request(textmove->desctext, 800,-1);				//wrap on the bound of the scrolled window


	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(cb_menu),textmove);
	
	/*scroll window
	*/
	scrolledwindow1 = gtk_scrolled_window_new(NULL,NULL);
	scrolledwindow2 = gtk_scrolled_window_new(NULL,NULL);
	scrolledwindow3 = gtk_scrolled_window_new(NULL,NULL);
	scrolledwindow4 = gtk_scrolled_window_new(NULL,NULL);
	scrolledwindow_all = gtk_scrolled_window_new(NULL,NULL);
	gtk_widget_set_size_request(scrolledwindow1, 430, 170);		//guide
	gtk_widget_set_size_request(scrolledwindow2, 430, 170);		//iotext
	gtk_widget_set_size_request(scrolledwindow3,800, 170);		//desctext
	gtk_widget_set_size_request(scrolledwindow4,700, 150);		//resulttext

	
	/*editable blank
	*/
	textmove->editor = gtk_entry_new(); 
	gtk_entry_set_text(GTK_ENTRY(textmove->editor),"Enter command for specified model HERE");
	

	/*button variable
	*/
	exit_button = gtk_button_new_with_label("Exit");
	input_button = gtk_button_new_with_label("Parse Input File");
	arc_button = gtk_button_new_with_label("model arc");
	deduction_button = gtk_button_new_with_label("Input Deduction");
	config_button = gtk_button_new_with_label("Parse config and create new input file");
	enter_button = gtk_button_new_with_label("Enter");
	g_signal_connect(G_OBJECT(exit_button), "clicked", G_CALLBACK(cb_qbutton),NULL);
	g_signal_connect(G_OBJECT(input_button), "clicked", G_CALLBACK(cb_ibutton),combo);
	g_signal_connect(G_OBJECT(arc_button), "clicked", G_CALLBACK(cb_abutton),NULL);
	g_signal_connect(G_OBJECT(deduction_button), "clicked", G_CALLBACK(cb_dbutton),NULL);
	g_signal_connect(G_OBJECT(enter_button), "clicked", G_CALLBACK(cb_ebutton),GTK_ENTRY(textmove->editor));
	g_signal_connect(G_OBJECT(config_button), "clicked", G_CALLBACK(cb_cbutton),combo);

	/*box
	*/
	vbox=gtk_vbox_new(false, 10);
	vbox_s3=gtk_vbox_new(false, 10);
	vbox_s2=gtk_vbox_new(false, 10);
	hbox_head = gtk_hbox_new(FALSE, 10);
	hbox1 = gtk_hbox_new(FALSE, 10);
	hbox2 = gtk_hbox_new(FALSE, 10);
	hbox3 = gtk_hbox_new(FALSE, 10);
	hbox4_2 = gtk_hbox_new(FALSE, 10);				//guid text box
	hbox4_1 = gtk_hbox_new(FALSE, 10);				//input text box
	hbox4 = gtk_hbox_new(FALSE, 10);				// box of the two above
	hbox_desc=gtk_hbox_new(FALSE,10);
	/*assmeble box
	*/
	 gtk_box_pack_start(GTK_BOX(hbox_head), headtext, TRUE, TRUE, 10);
	 gtk_box_pack_start(GTK_BOX(hbox1), textmove->editor, TRUE, TRUE, 10);
	 gtk_box_pack_start(GTK_BOX(hbox3), exit_button, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox3), enter_button, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox2), combotext, false, false, 3);
	 gtk_box_pack_start(GTK_BOX(hbox2), combo, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox2), arc_button, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox_desc), textmove->desctext, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox4_2), textmove->text, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox4_1), textmove->iotext, false, false, 10);	 
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow1),hbox4_2);
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow2),hbox4_1);
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow3),hbox_desc);
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow4),result_text);
	 gtk_box_pack_start(GTK_BOX(vbox_s2), hbox2, false, false, 3);
	 gtk_box_pack_start(GTK_BOX(vbox_s2), scrolledwindow3, false, false, 3);
	 gtk_box_pack_start(GTK_BOX(hbox4), scrolledwindow1, false, false, 5);
	 gtk_box_pack_start(GTK_BOX(hbox4), scrolledwindow2, false, false, 5);
	 gtk_box_pack_start(GTK_BOX(vbox_s3), hbox4, false, false, 3);
	 gtk_box_pack_start(GTK_BOX(vbox_s3), hbox1, false, false, 3);
	 gtk_box_pack_start(GTK_BOX(vbox_s3), hbox3, false, false, 3);



	 /*button box
	 */
	buttonbox1=gtk_hbutton_box_new ();
	gtk_container_add(GTK_CONTAINER(buttonbox1), config_button); 	 
	gtk_container_add(GTK_CONTAINER(buttonbox1), input_button); 
	gtk_container_add(GTK_CONTAINER(buttonbox1), deduction_button);
	gtk_button_box_set_layout (GTK_BUTTON_BOX(buttonbox1),GTK_BUTTONBOX_SPREAD);
	gtk_box_set_spacing (GTK_BOX(buttonbox1),5);
	
	/*frame
	*/
	frame1=gtk_frame_new ("Step1. Parse configure/input file");
	frame2=gtk_frame_new ("Step2. Choose a model and read the description");	
	frame3=gtk_frame_new ("Step3. Enter command as instruction");	
	frame4=gtk_frame_new ("Result");	
	gtk_container_add(GTK_CONTAINER(frame1), buttonbox1); 	
	gtk_container_add(GTK_CONTAINER(frame2), vbox_s2); 
	gtk_container_add(GTK_CONTAINER(frame3), vbox_s3); 	 
	gtk_container_add(GTK_CONTAINER(frame4), scrolledwindow4); 
	
		
			
	/*assmeble frame
	*/
	 gtk_box_pack_start(GTK_BOX(vbox), hbox_head, false, false,5);	 
	 gtk_box_pack_start(GTK_BOX(vbox), frame1, false, false,5);	 
	 gtk_box_pack_start(GTK_BOX(vbox), frame2, false, false,5);
	 gtk_box_pack_start(GTK_BOX(vbox), frame3, false, false,5);
	 gtk_box_pack_start(GTK_BOX(vbox), frame4, false, false,5);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow_all),vbox); 
	
	gtk_container_add(GTK_CONTAINER(main_window), scrolledwindow_all); 
	gtk_widget_show_all(main_window); 
	gtk_window_move(GTK_WINDOW(main_window),100,100);
    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();
	 return 0;
}
