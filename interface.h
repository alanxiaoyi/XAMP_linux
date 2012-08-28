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
#define LENGTH 4

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
	string output[io_num][3];	 //output name and content as 3d array
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