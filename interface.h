#ifndef INTERFACE_H
#define INTERFACE_H
#include "math.h"
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <list>
#include <string>
#include <sstream>
#include <cstring>
#include <errno.h>
#include <signal.h>

#define command_param_num 40
#define io_num 20
using namespace std;

int call_gui();
int parse_config(const char *, bool);
int parse_input_file(const char *);
int check_ready();
int construct_menu();
int input_deduction();

class model_class{
public:
	int num;
	string name;			 //model name
	char dir[256];			 //directory path of the model
	string input[io_num][4];	 //input name, content,  comment, and primaryornot as 4th array
	string output[io_num][2];	 //output name and content as 2d array
	string comment;			 //discription of the model
	string guide;            //user guide of the model
	string dft[io_num];           //sets of defaults 
	int ready;
    model_class();
};

class input_class{
public:
	input_class();
	string name;
	string comment;
	string content;
};
extern int init_model(list<model_class>::iterator , int, string );
extern list<model_class> model_list;
extern list<input_class> input_list;
extern const char* config_file_name;
extern const char* input_file_name;
extern int create_input_file(list<input_class> );

#endif