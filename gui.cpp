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


extern int find_model_arc(list<string>*, string, int);			//from model_arc
extern int reverse_find_model_arc(list<string>*,int);			//from model_arc
extern int forward_find_model_arc(list<string>*,int);			//from model_arc
extern list<model_class> allcandidate[LENGTH];								//from model_arc

int call_yuml();

list<string> input_name_list;
list<string> output_name_list;
string output_name;

using namespace std;

//text box struct in main window
typedef struct{
GtkWidget *text;
GtkWidget *iotext;
GtkWidget *desctext;
GtkWidget *assumtext;
GtkWidget	*editor;
}textstruct;

textstruct *textmove;

GtkWidget *main_window, *result_text, *second_window, *uml_window; 
GtkTreeSelection *selection_in, *selection_out;
GtkTreeSelection *selection_result[4];
GtkWidget *list_result[4],*comment_text,*assumption_text, *io_text;			//text boxes for second window
GtkListStore *store_result[4];
GtkWidget *combo_iobase, *combo_depth;
GtkWidget *command_box[4], *tag_box[4];
GtkWidget *combo_ops[4];
GtkWidget *ops_box[4];
GtkWidget *combo;

int combo_count=0;
list<model_class>::iterator themodel;
GThread *thread;





/*exevc yuml to generate uml figure*/

int call_yuml(){

	char* uml_carg[6];

	uml_carg[0]=(char*)(yuml);
	uml_carg[1]=(char*)("-i");
	uml_carg[2]=(char*)(uml_text_file);
	uml_carg[3]=(char*)("-o");
	uml_carg[4]=(char*)(uml_graph_file);
	uml_carg[5]=NULL;

	pid_t uml_pID = fork();          //fork new process to call a model
	if (uml_pID == 0){	

		int i=execv(yuml,  uml_carg);	

		if(-1==i){
			cout<<"fail to execute yuml"<<endl;
			perror("error is");
			_exit(1);
		}
	}
	else if (uml_pID < 0){
        	cout << "Failed to fork" << endl;
       		 exit(1);
	}
	else{	
		int status;
		pid_t pid;
		pid=wait(&status);
    	}

	return 1;

}

/*processing_dialog
*/

void close_processing_dialog( GtkWidget *widget){
	g_print("You stop the process\n"); 
	exit(0);
}


int find_model_num(string model_name){

	if(model_name.find('*')==0)				//if there is "*", remove it before campare
		model_name=model_name.substr(1,-1);	
	list<model_class>::iterator it;
	int model_num;
	for(it=model_list.begin(); it!=model_list.end(); it++){				
		if(it->name==model_name){		
			model_num=it->num;
			break;
		}
	}	

	return model_num;

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

  store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));

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


/*get input output information string*/
string get_ioinfo_string(list<model_class>::iterator itm){

	string tmp;
	int n=0;
	tmp="input: \n\n";
	while(itm->input[n][0]!=""){
		tmp=tmp+itm->input[n][0]+": "+itm->input[n][1]+"\t\t//"+itm->input[n][2]+"\n";
		n++;			
	}
		n=0;
		tmp=tmp+"\n\noutput: \n\n";
	while(itm->output[n][0]!=""){
		tmp=tmp+itm->output[n][0]+"\t// can derive out: "+itm->output[n][2]+"\t\t//"+itm->output[n][1]+"\n";
		n++;
			
	}

	return tmp;
}



/*model select callback
*/
void tree_selection_changed_cb (GtkTreeSelection *selection, gpointer data)
{
        GtkTreeIter iter;
        GtkTreeModel *model;
        char *name;
	string tmp;
	list<model_class>::iterator it;

        if (gtk_tree_selection_get_selected (selection, &model, &iter))
        {
                gtk_tree_model_get (model, &iter, 0, &name, -1);
				string name_s=string(name);
				if(name_s.find('*')==0)				//if there is "*", remove it before campare
					name_s=name_s.substr(1,-1);				
                cout<<"you select "<<name_s<<endl;
				for(it=model_list.begin(); it!=model_list.end(); it++){
					if(it->name==name_s){					//find the model by name
							gtk_label_set_text(GTK_LABEL(comment_text),("Description of the model:\n"+it->comment).c_str());	
							gtk_label_set_text(GTK_LABEL(assumption_text),("Assumption used by the model:\n"+it->assumption).c_str());
							gtk_label_set_text(GTK_LABEL(io_text),get_ioinfo_string(it).c_str());
	
					}
	
				}
        }			
}


/*wrap the topology figure window*/
void wrap_topology_window(){


	GtkWidget *scrolled_image;
	scrolled_image= gtk_scrolled_window_new(NULL,NULL);

	uml_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(uml_window), "UML");	
	gtk_window_set_default_size(GTK_WINDOW(uml_window), 1300,700); 	
	gtk_window_set_policy (GTK_WINDOW(uml_window), true, true, true);

	GtkWidget *image;
	image = gtk_image_new_from_file (uml_graph_file);

	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_image),image);	
	gtk_container_add(GTK_CONTAINER(uml_window), scrolled_image);
	gtk_widget_show_all(uml_window);

}

/*search arc button
*/
void cb_search_button(GtkWidget *widget, gpointer data) {
	 input_name_list.clear();
	 output_name_list.clear();
	 for(int i=0; i<4; i++){
		GtkListStore *store;
		store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list_result[i])));
		gtk_list_store_clear(store);
	 }
	 output_name="";
	 gtk_tree_selection_selected_foreach(selection_in, view_selected_in_foreach_func, NULL);			
	 gtk_tree_selection_selected_foreach(selection_out, view_selected_out_foreach_func, NULL);
	 
	int temp_depth=gtk_combo_box_get_active (GTK_COMBO_BOX(combo_depth));		// the depth to search
	
	for(int i=0; i<LENGTH; i++){
		allcandidate[i].clear();
	}
	
	if(!input_name_list.empty() &&	gtk_combo_box_get_active (GTK_COMBO_BOX(combo_iobase))==0)
		int a = find_model_arc(&input_name_list, output_name, temp_depth);	

	else if( !input_name_list.empty()  &&	gtk_combo_box_get_active (GTK_COMBO_BOX(combo_iobase))==1){
		int a = forward_find_model_arc(&input_name_list,temp_depth);
	}
	
	else if( output_name!="" &&	gtk_combo_box_get_active (GTK_COMBO_BOX(combo_iobase))==2){
		output_name_list.push_back(output_name);
		int a = reverse_find_model_arc(&output_name_list,temp_depth);
	}
	

		
	list<model_class>::iterator it;
	
//	if(gtk_combo_box_get_active (GTK_COMBO_BOX(combo_iobase))==0){				//input/output based
		for(int i=0; i<LENGTH; i++){
			if(!allcandidate[i].empty()){
				for(it=allcandidate[i].begin(); it!=allcandidate[i].end(); it++){		
					stringstream ss;
					ss<<it->num;							
					add_to_list(list_result[i], (it->name).c_str());
				}			
			}	
		}	
//	}
	
/*	else if(gtk_combo_box_get_active (GTK_COMBO_BOX(combo_iobase))==1){				//input based
		for(int i=0; i<temp_depth+1; i++){
			if(!allcandidate[i].empty()){
				for(it=allcandidate[i].begin(); it!=allcandidate[i].end(); it++){		
					stringstream ss;
					ss<<it->num;							
					add_to_list(list_result[i], (it->name).c_str());
				}			
			}	
		}	
	}
	
	
	else{												//output based
		for(int i=0; i<temp_depth+1; i++){
			if(!allcandidate[i].empty()){
				for(it=allcandidate[i].begin(); it!=allcandidate[i].end(); it++){		
					stringstream ss;
					ss<<it->num;							
					add_to_list(list_result[i], (it->name).c_str());
				}			
			}	
		}	

	}*/



	GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(second_window),
        			GTK_DIALOG_DESTROY_WITH_PARENT,
       				 GTK_MESSAGE_QUESTION,
       				 GTK_BUTTONS_YES_NO,
                                 "A textual UML file descripting the topology is created in sub directory /UML/.\n Do you want XAMP to send it to yUML server(yUML.me) to automatically generate a UML figure?");
				gint result = gtk_dialog_run (GTK_DIALOG (dialog));				//get the user's choice
				gtk_widget_destroy (dialog);

	if(result==-8) {
		
		call_yuml();	
		wrap_topology_window();
	
	}
	
}

/*assemble pipe command
*/

string assemble_pipe_cmd(string model_command_tag[][5], int level, string sub){
		list<model_class>::iterator it;
		int model_num;
		if(model_command_tag[level][0]=="")
			return "";
		


		model_num=find_model_num(model_command_tag[level][0]);

			
		if(level!=3){
			if(model_command_tag[level+1][0]!=""){
				
				if(model_command_tag[level][1].find("*")!=-1)
					model_command_tag[level][1].replace(model_command_tag[level][1].find("*"), 1, sub);   
				stringstream ss;
				ss<<model_num;
				string a="[/"+model_command_tag[level][2]+"/"+" -pipefrom "+ss.str()+" -cmd "+model_command_tag[level][1]+" -ops "+model_command_tag[level][3]+" "+model_command_tag[level][4]+"]";				
				return assemble_pipe_cmd(model_command_tag, level+1, a);				
			}
			else {
				if(model_command_tag[level][1].find("*")!=-1)
					model_command_tag[level][1].replace(model_command_tag[level][1].find("*"), 1, sub);   
				string a= model_command_tag[level][1];
				return a;
			}
		
		}
		
		else {
			if(model_command_tag[level][1].find("*")!=-1)
				model_command_tag[level][1].replace(model_command_tag[level][1].find("*"), 1, sub);   	
			string a= model_command_tag[level][1];
			return a;
		
		}

}

/*pipe button
*/

void cb_pipe_button(GtkWidget *widget, gpointer data) {
       
	GtkTreeIter iter;
    GtkTreeModel *model;
	char *name;
	string last_model;
	int n=0;
	
	string model_command_tag[4][5];
	for(int i=0; i<4; i++)
		for (int j=0; j<5; j++)
			model_command_tag[i][j]="";
	
	
	for (int i=0; i<4; i++){
	
		if (gtk_tree_selection_get_selected (selection_result[i], &model, &iter)){
            gtk_tree_model_get (model, &iter, 0, &name, -1);
			string name_s=string(name);
			
			string ops_name;
			string ops_num;
			switch (gtk_combo_box_get_active (GTK_COMBO_BOX(combo_ops[i]))){
	
				case 0: ops_name="times";
						break;
				case 1: ops_name="divby";
						break;
				case 2: ops_name="plus";
						break;
				case 3: ops_name="pow";
						break;
				default: break;				
			}
			ops_num=gtk_entry_get_text(GTK_ENTRY(ops_box[i]));
			if (name_s!=""){
				model_command_tag[n][0]=name_s;
				model_command_tag[n][1]=gtk_entry_get_text(GTK_ENTRY(command_box[i]));
				model_command_tag[n][2]=gtk_entry_get_text(GTK_ENTRY(tag_box[i]));
				model_command_tag[n][3]=ops_name;
				model_command_tag[n][4]=ops_num;
				last_model=name_s;
				n++;		
			}
			else if(name_s=="" && n>0){
			
				break;
			
			}
		}
	}
	

	int model_num=find_model_num(last_model);
	
	string temp = assemble_pipe_cmd(model_command_tag, 0 , "");

	cout<<temp<<endl;
	gtk_combo_box_set_active            (GTK_COMBO_BOX(combo),
                                                         model_num-1);
	gtk_entry_set_text(GTK_ENTRY(textmove->editor),temp.c_str());
}		


/*arc button	/construct the second window here  FIXME:better implement this outside the call back function
*/
void cb_abutton(GtkWidget *widget, gpointer data) {
	g_print("push arc_button\n"); 
	GtkWidget *vbox_combo,*vbox,*vbox_step2, *hbox,*hbox_desc_assum,*hbox_result,*arrow_frame[3], *searching_arc_button;
	GtkWidget *scrolled_text,*scrolled_assum_text, *scrolled_io_text, *scrolledwindow_in,*scrolledwindow_out, *scrolled_result[4];
	GtkWidget *arrow[3];
	GtkWidget *hbox_command;
	GtkWidget *pipe_button;
	GtkWidget *hbox_searching_arc_button, *hbox_pipe_button;

	GtkWidget *step_frame[4];
	GtkWidget *combo_io=gtk_label_new("Query Type:");
	GtkWidget *combo_dp=gtk_label_new("Query Depth:");





	//pipe button
	
	
	pipe_button = gtk_button_new_with_label("Launch");	
	g_signal_connect(G_OBJECT(pipe_button), "clicked", G_CALLBACK(cb_pipe_button),NULL);	
	
	
	//combo box

	combo_iobase=gtk_combo_box_new_text ();
	combo_depth=gtk_combo_box_new_text ();
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_iobase), ("input/output based"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_iobase), ("input based"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_iobase), ("output based"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_depth), ("1"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_depth), ("2"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_depth), ("3"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_depth), ("4"));
	gtk_combo_box_set_active            (GTK_COMBO_BOX(combo_iobase), 0);
	gtk_combo_box_set_active            (GTK_COMBO_BOX(combo_depth), 3);	
	
	
                                                      
	//comment box
	comment_text=gtk_label_new("");
	assumption_text=gtk_label_new("");
	io_text=gtk_label_new("");
	
	scrolled_text= gtk_scrolled_window_new(NULL,NULL);
	gtk_widget_set_size_request(scrolled_text, 600, 170);		
	scrolled_assum_text= gtk_scrolled_window_new(NULL,NULL);
	gtk_widget_set_size_request(scrolled_assum_text, 600, 170);
	scrolled_io_text= gtk_scrolled_window_new(NULL,NULL);
	gtk_widget_set_size_request(scrolled_io_text, 600, 170);
	
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_text),comment_text);	
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_assum_text),assumption_text);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_io_text),io_text);

	
	gtk_label_set_line_wrap (GTK_LABEL(comment_text), true);	
	gtk_label_set_line_wrap (GTK_LABEL(assumption_text), true);	
	for(int i=0; i<3; i++){		
		arrow_frame[i]=gtk_frame_new ("");	
		arrow[i]=gtk_arrow_new(GTK_ARROW_RIGHT,GTK_SHADOW_OUT);	
	}
	

	
	//input output listview
	scrolledwindow_in = gtk_scrolled_window_new(NULL,NULL);
	scrolledwindow_out = gtk_scrolled_window_new(NULL,NULL);

	gtk_widget_set_size_request(scrolledwindow_in, 400, 220);		//guide
	gtk_widget_set_size_request(scrolledwindow_out, 400, 220);		//iotext	
	
	
	hbox_searching_arc_button = gtk_hbox_new(FALSE, 10);
	hbox_pipe_button = gtk_hbox_new(FALSE, 10);
	vbox = gtk_vbox_new(FALSE, 0);
	vbox_combo = gtk_vbox_new(FALSE, 0);
	vbox_step2=gtk_vbox_new(FALSE,0);
	hbox = gtk_hbox_new(FALSE, 0);
	hbox_desc_assum=gtk_hbox_new(FALSE, 0);
	hbox_command=gtk_hbox_new(FALSE, 0);
	
	hbox_result = gtk_hbox_new(FALSE, 0);		
	list<string> output_treeview_list, input_treeview_list;

	
	searching_arc_button = gtk_button_new_with_label("Construct model connection topology");	
	g_signal_connect(G_OBJECT(searching_arc_button), "clicked", G_CALLBACK(cb_search_button),NULL);
	
	list<model_class>::iterator itm;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column_in, *column_out, *column_result[4];
	GtkListStore *store_in, *store_out, *store_result[4];
	GtkWidget *list_in, *list_out;
	renderer = gtk_cell_renderer_text_new();
	
	//4 result listview box and command box
	for(int i=0; i<4; i++){
		scrolled_result[i] = gtk_scrolled_window_new(NULL,NULL);
		gtk_widget_set_size_request(scrolled_result[i], 200, 150);		
		column_result[i]=gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
		list_result[i]=gtk_tree_view_new();
		gtk_tree_view_append_column(GTK_TREE_VIEW(list_result[i]), column_result[i]);
		store_result[i]=gtk_list_store_new(1, G_TYPE_STRING); 
		gtk_tree_view_set_model(GTK_TREE_VIEW(list_result[i]), GTK_TREE_MODEL(store_result[i]));
		selection_result[i] = gtk_tree_view_get_selection(GTK_TREE_VIEW(list_result[i]));
		g_signal_connect (G_OBJECT (selection_result[i]), "changed",
				  G_CALLBACK (tree_selection_changed_cb),
                  NULL);	
	
		gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_result[i]),list_result[i]);	


		//command box, tag box, ops combo, ops box
		command_box[i] = gtk_entry_new(); 
		gtk_entry_set_text(GTK_ENTRY(command_box[i]),"Enter Command");	
		gtk_entry_set_width_chars (GTK_ENTRY(command_box[i]), 20);
		
		tag_box[i] = gtk_entry_new(); 
		gtk_entry_set_text(GTK_ENTRY(tag_box[i]),"tag");	
		gtk_entry_set_width_chars (GTK_ENTRY(tag_box[i]), 2);	

		ops_box[i] = gtk_entry_new(); 
		gtk_entry_set_text(GTK_ENTRY(ops_box[i]),"ops");	
		gtk_entry_set_width_chars (GTK_ENTRY(ops_box[i]), 2);	

		combo_ops[i]=gtk_combo_box_new_text ();
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_ops[i]), ("times"));
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_ops[i]), ("divby"));
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_ops[i]), ("plus"));
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_ops[i]), ("pow"));
		gtk_combo_box_set_active (GTK_COMBO_BOX(combo_ops[i]), 0);
		
	}
	

	
	
	
	column_in = gtk_tree_view_column_new_with_attributes("Input List", renderer, "text", 0, NULL);
	column_out = gtk_tree_view_column_new_with_attributes("Output List", renderer, "text", 0, NULL);	
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
	gtk_window_set_title(GTK_WINDOW(second_window), "Seeking Model Chain");	
	gtk_window_set_default_size(GTK_WINDOW(second_window), 1400,830); 	
	gtk_window_set_policy (GTK_WINDOW(second_window), true, true, true);



	//fill in the treeview: input list and output list
	for(itm=model_list.begin(); itm!=model_list.end();itm++){
		int n=0;
		int m=0;
		while(itm->output[n][0]!=""){
			output_treeview_list.push_back(itm->output[n][0]);
			n++;
		}
		while(itm->input[m][0]!=""){
			if(itm->input[m][3]=="y"){				
				input_treeview_list.push_back(itm->input[m][0]);
			}
			m++;
		}
	}
	output_treeview_list.sort();
	output_treeview_list.unique();
	input_treeview_list.sort();
	input_treeview_list.unique();

	list<string>::iterator sit;

	for(sit=output_treeview_list.begin();sit!=output_treeview_list.end();sit++){		
		add_to_list(list_out, (*sit).c_str());
	}
	for(sit=input_treeview_list.begin();sit!=input_treeview_list.end();sit++){		
		add_to_list(list_in, (*sit).c_str());
	}	
		

	gtk_tree_view_set_enable_search (GTK_TREE_VIEW(list_in),true);
	gtk_tree_view_set_search_column (GTK_TREE_VIEW(list_in), 0);
	

		
		
		
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow_in),list_in);
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow_out),list_out);
	for(int i=0; i<3; i++){
		gtk_container_add (GTK_CONTAINER (arrow_frame[i]), arrow[i]);
	}	
	
	
	
	for(int i=0; i<3; i++){
		gtk_box_pack_start(GTK_BOX(hbox_command), command_box[i], TRUE, TRUE, 5);		
		gtk_box_pack_start(GTK_BOX(hbox_command), tag_box[i], TRUE, TRUE, 5);
		gtk_box_pack_start(GTK_BOX(hbox_command), combo_ops[i], TRUE, TRUE, 5);
		gtk_box_pack_start(GTK_BOX(hbox_command), ops_box[i], TRUE, TRUE, 5);	
	}
	
	gtk_box_pack_start(GTK_BOX(hbox_command), command_box[3], TRUE, TRUE, 5);


	 gtk_box_pack_start(GTK_BOX(hbox_pipe_button), pipe_button, false, false, 10);	
	 gtk_box_pack_start(GTK_BOX(hbox_searching_arc_button), searching_arc_button, false, false, 10);		
		
	gtk_box_pack_start(GTK_BOX(vbox_combo), combo_io, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox_combo), combo_iobase, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox_combo), combo_dp, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox_combo), combo_depth, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), scrolledwindow_in, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), vbox_combo, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), scrolledwindow_out, TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(hbox_result), scrolled_result[0], TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox_result), arrow_frame[0], TRUE, TRUE, 5);		
	gtk_box_pack_start(GTK_BOX(hbox_result), scrolled_result[1], TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(hbox_result), arrow_frame[1], TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(hbox_result), scrolled_result[2], TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox_result), arrow_frame[2], TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(hbox_result), scrolled_result[3], TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox_step2), hbox_searching_arc_button, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox_step2), hbox_result, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox_desc_assum), scrolled_text, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox_desc_assum), scrolled_io_text, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox_desc_assum), scrolled_assum_text, TRUE, TRUE, 5);

	step_frame[0]=gtk_frame_new ("Step 1: Choose a query and select available inputs and desired outputs as applicable.");	
	step_frame[1]=gtk_frame_new ("Step 2: Run the query engine to construct model connection topology, then select models to connect.");	
	step_frame[2]=gtk_frame_new ("Step 3: Enter a command for each selected model, and specify the output tag of a model as input to the next model. \nOutput may be arithmetically transformed (optional).");	
	step_frame[3]=gtk_frame_new ("Step 4: Launch the selected models. ");		
	gtk_container_add(GTK_CONTAINER(step_frame[0]), hbox); 	
	gtk_container_add(GTK_CONTAINER(step_frame[1]), vbox_step2); 
	gtk_container_add(GTK_CONTAINER(step_frame[2]), hbox_command);
	gtk_container_add(GTK_CONTAINER(step_frame[3]), hbox_pipe_button);



	
	gtk_box_pack_start(GTK_BOX(vbox), step_frame[0], TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(vbox), step_frame[1], TRUE, TRUE, 5);	
	gtk_box_pack_start(GTK_BOX(vbox), step_frame[2], TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), step_frame[3], TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox_desc_assum, TRUE, TRUE, 5);
	
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
	GtkWidget *otherwgt5=(GtkWidget*) (data->assumtext);
	int a=gtk_combo_box_get_active (combo);
	string tmp;
	for(itm=model_list.begin(); itm!=model_list.end();itm++){
		if(itm->num==a+1) break;
	}
	if(itm!=model_list.end()){
		themodel=itm;
		int n=0;
		tmp=get_ioinfo_string(itm);

		gtk_entry_set_text (GTK_ENTRY(otherwgt4), (itm->dft[0]).c_str());
		gtk_label_set_text(GTK_LABEL(otherwgt2),tmp.c_str());
		gtk_label_set_text(GTK_LABEL(otherwgt1), ("User Guide:\n\n"+itm->guide).c_str());
		gtk_label_set_text(GTK_LABEL(otherwgt3), ("Description of the model:\n\n"+itm->comment).c_str());
		gtk_label_set_text(GTK_LABEL(otherwgt5), ("Assumption used by the model:\n\n"+itm->assumption).c_str());
	}
}

int call_gui() { 
	g_thread_init(NULL);
   	gdk_threads_init();
	gtk_init(NULL, NULL);
	GtkWidget *hbox_head,*hbox1, *hbox2, *hbox3 ,*hbox4,*hbox_arc_button, *hbox4_1,*hbox4_2,*hbox_desc,*hbox_assum ,*vbox, *hbox_desc_assum;
	GtkWidget *arc_button, *exit_button,*input_button,*deduction_button,*enter_button,*config_button, *combotext,*headtext;
	GtkWidget *buttonbox1,*frame0, *frame1, *frame2, *frame3, *frame4, *frame5, *frame6, *vbox_s3,*vbox_s2;
	GtkWidget *scrolledwindow1,*scrolledwindow2,*scrolledwindow3,*scrolledwindow4,*scrolledwindow5, *scrolledwindow_all;
	GtkObject *vadjust,*hadjust;
	textmove=g_slice_new(textstruct);

		 
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(main_window), "destroy", G_CALLBACK(gtk_main_quit), NULL); 
	gtk_window_set_title(GTK_WINDOW(main_window), "XAMP"); 
	gtk_window_set_default_size(GTK_WINDOW(main_window), 980,830); 
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
	textmove->assumtext=gtk_label_new("");
	result_text=gtk_label_new("");
	gtk_label_set_selectable (GTK_LABEL(result_text), true);
	headtext=gtk_label_new("\t\t\t**********Welcome to XAMP********** \nCopyright (c)2012, Yan Solihin and Yipeng Wang. All Rights Reserved.");
//	gtk_widget_set_size_request(  headtext , 700, -1 );
//	gtk_label_set_line_wrap (GTK_LABEL(headtext), true);	
	combotext=gtk_label_new("Choose model:");
	gtk_label_set_line_wrap( GTK_LABEL( result_text), TRUE );
	gtk_widget_set_size_request(  result_text , 900, -1 );
	gtk_label_set_line_wrap (GTK_LABEL(textmove->text), true);			//guide text wrap
	gtk_widget_set_size_request(textmove->text, 370,-1);				//wrap on the bound of the scrolled window
	gtk_label_set_line_wrap (GTK_LABEL(textmove->desctext), true);		//description text wrap
	gtk_widget_set_size_request(textmove->desctext, 440,-1);				//wrap on the bound of the scrolled window
	gtk_label_set_line_wrap (GTK_LABEL(textmove->assumtext), true);		//description text wrap
	gtk_widget_set_size_request(textmove->assumtext, 440,-1);				//wrap on the bound of the scrolled window
	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(cb_menu),textmove);
	
	/*scroll window
	*/
	scrolledwindow1 = gtk_scrolled_window_new(NULL,NULL);
	scrolledwindow2 = gtk_scrolled_window_new(NULL,NULL);
	scrolledwindow3 = gtk_scrolled_window_new(NULL,NULL);
	scrolledwindow4 = gtk_scrolled_window_new(NULL,NULL);
	scrolledwindow5 = gtk_scrolled_window_new(NULL,NULL);
	scrolledwindow_all = gtk_scrolled_window_new(NULL,NULL);
	gtk_widget_set_size_request(scrolledwindow1, 455, 170);		//guide
	gtk_widget_set_size_request(scrolledwindow2, 460, 170);		//iotext
	gtk_widget_set_size_request(scrolledwindow3,455, 170);		//desctext
	gtk_widget_set_size_request(scrolledwindow5,460, 170);		//assumtext
	gtk_widget_set_size_request(scrolledwindow4,700, 150);		//resulttext

	
	/*editable blank
	*/
	textmove->editor = gtk_entry_new(); 
	gtk_entry_set_text(GTK_ENTRY(textmove->editor),"Enter command for specified model HERE");
	

	/*button variable
	*/
	exit_button = gtk_button_new_with_label("Exit");
	input_button = gtk_button_new_with_label("Parse Input File");
	arc_button = gtk_button_new_with_label("Connect Multiple Models");
	deduction_button = gtk_button_new_with_label("Deduce Additional Inputs");
	config_button = gtk_button_new_with_label("Parse Config and Create New Input File");
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
	hbox_desc_assum=gtk_hbox_new(FALSE,10);
	hbox3 = gtk_hbox_new(FALSE, 10);
	hbox_arc_button=gtk_hbox_new(FALSE,10);
	hbox4_2 = gtk_hbox_new(FALSE, 10);				//guid text box
	hbox4_1 = gtk_hbox_new(FALSE, 10);				//input text box
	hbox4 = gtk_hbox_new(FALSE, 10);				// box of the two above
	hbox_desc=gtk_hbox_new(FALSE,10);
	hbox_assum=gtk_hbox_new(FALSE,10);
	/*assmeble box
	*/
	 gtk_box_pack_start(GTK_BOX(hbox_head), headtext, TRUE, TRUE, 10);
	 gtk_box_pack_start(GTK_BOX(hbox1), textmove->editor, TRUE, TRUE, 10);
	 gtk_box_pack_start(GTK_BOX(hbox3), exit_button, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox3), enter_button, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox2), combotext, false, false, 3);
	 gtk_box_pack_start(GTK_BOX(hbox2), combo, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox_arc_button), arc_button, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox_desc), textmove->desctext, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox_assum), textmove->assumtext, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox4_2), textmove->text, false, false, 10);
	 gtk_box_pack_start(GTK_BOX(hbox4_1), textmove->iotext, false, false, 10);	 
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow1),hbox4_2);
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow2),hbox4_1);
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow3),hbox_desc);
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow5),hbox_assum);
	 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow4),result_text);
	 
	 
	 gtk_box_pack_start(GTK_BOX(hbox_desc_assum), scrolledwindow3, false, false, 3);	 
	 gtk_box_pack_start(GTK_BOX(hbox_desc_assum), scrolledwindow5, false, false, 3);
	 
	 gtk_box_pack_start(GTK_BOX(vbox_s2), hbox2, false, false, 3);
	 gtk_box_pack_start(GTK_BOX(vbox_s2), hbox_arc_button, false, false, 3);	
	 gtk_box_pack_start(GTK_BOX(vbox_s2), hbox_desc_assum, false, false, 3);
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
	frame1=gtk_frame_new ("Step 1: \nParse configuration and input files.\nClick \"Parse Input File\" if you want to read in new input values into XAMP (optional).\nClick \"Deduce Additional Inputs\" to derive additional inputs from existing inputs (optional).\n");
	frame2=gtk_frame_new ("Step2. Choose a model");	
	frame3=gtk_frame_new ("Step3. Enter command");	
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






