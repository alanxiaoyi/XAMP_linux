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
#include "interface.h"
#include <cstdio>

using namespace std;
list<model_class> model_list;
list<input_class> input_list;
const char* config_file_name="config.xml";
const char* input_file_name="input.xml";
int recur_counter=0;
int call_model(char **);
int init_model(list<model_class>::iterator , int, string );
int read_pipe(string, string*, string);
int parse_cmd(string*);
int help();
list<model_class>::iterator find_model(int);

int release_mem_pp(char** carg){
	for(int i=0; i<command_param_num; i++)
		if(carg[i]!=NULL){
			delete[] carg[i];
			carg[i]=NULL;
		}
}

/* main function
*/
int main(int argc, char *argv[]){	
	call_gui();
	return 1;
}

/*responsible for parse command line and call the model
*/
int init_model(	list<model_class>::iterator it,  int waitornot, string cmd){
	recur_counter++;
	char* carg[command_param_num];
	string s;
	int n=1;
	char cmd_param[256];
	for(int i=0;i<command_param_num;i++){						//initialize 
		carg[i]=new char[256];
	}
	
	if(waitornot==1){											//show comment and wait for user to enter parameter, used in menu style 
		cout<<"comment:"<<endl;
		cout<<it->comment<<endl;   				    			//cout the comment(help) of the model	
		cout<<it->guide<<endl;
		cout<<"please enter the parameter the model needs below, or enter \"back\" to go back"<<endl;
		cin.getline(cmd_param, 256);   							//get the parameters
		string str=cmd_param;
		stringstream ss(str);
		while(ss>>s){											// split the parameters and save them to carg[n]
			if(1==n && "back"==s) return 0;
			carg[n]=new char [s.size()+1];
			strcpy(carg[n],s.c_str());
			n++;
		}
	}	
	else if(waitornot==0){										//directly call the model with the default inputs. i.e. -c <number of model> <command line parameters>
		parse_cmd(&cmd);
		cout<<"The command you use is:"<<" "<<cmd<<endl;
		if(cmd.find('*')!=string::npos) {cout<<"should not have * here"<<endl; exit(1);}					//check status
		stringstream ss(cmd);
		while(ss>>s){											// split the parameters and save them to carg[n]
			carg[n]=new char [s.size()+1];
			strcpy(carg[n],s.c_str());
			n++;
		}		
	}

	/*finish carg[] and call the model*/
	strcpy(carg[0],it->dir);
	carg[n]=(char *) 0;
	for(int i=0; i<256;i++){
		if(carg[0][i]==13||carg[0][i]==10){                	   //replace \r \n with \0
			carg[0][i]=0;
			break;
		}		
	}
	cout<<"you call the model:"<<endl;
	cout<<it->name<<endl;
	cout<<"recursion level: "<<recur_counter-1<<endl;
	call_model(carg);
	
//	for(int i=0; i<command_param_num; i++)
//		delete[] carg[i];
	recur_counter--;
	return 1;
}

int parse_cmd(string *cmd){
		int pflag;
		char cmd_c[256];
		int found_l;
		int found_r;		
		string sub_line;
		string pipe_tag;
		int sub_model;
		string sub_cmd;
		string ops;
		string subst;					
	
		while((*cmd).find("[")!=string::npos){				//check if there are submodels which havn't been solved
			/*find '[' and ']' and then save their positions to found_l and found_r.*/
			string tmp_s;
			pflag=0;
			found_l=-1;
			found_r=-1;
//			cmd_c = new char[(*cmd).size()+1] ;
			strcpy(cmd_c, (*cmd).c_str());
			for(int i=0; i<strlen(cmd_c); i++){
				if(cmd_c[i]=='[') { 
					pflag++;
					if(pflag==1) found_l=i;
				}
				if(cmd_c[i]==']'){ 
					pflag--;
					if(pflag==0) found_r=i;
				}
				if(found_l!=-1 && found_r !=-1) break;		
			}
			if(found_r==-1) {cout<<"brackets are not pair!"<<endl; exit(1);}
			
			/*parse different part of sub_line(part between []) to variable*/
			sub_line=(*cmd).substr(found_l+1, found_r-found_l-1);
			cout<<"sub_line: "<<sub_line<<endl;
			int found_ls=sub_line.find("/");
			int found_rs=sub_line.find("/", found_ls+1);
			pipe_tag=sub_line.substr(found_ls+1, found_rs-found_ls-1);		
			stringstream ss(sub_line.substr(sub_line.find("-pipefrom"), string::npos));
			ss>>tmp_s;
			assert(tmp_s=="-pipefrom") ;
			ss>>sub_model;
			ss>>tmp_s;
			assert(tmp_s=="-cmd");					
			sub_cmd=sub_line.substr(sub_line.find("-cmd")+4, sub_line.rfind("-ops")-5-sub_line.find("-cmd"));			
			assert(sub_line.rfind("-ops")!=string::npos);
			ops=sub_line.substr(sub_line.rfind("-ops")+4, string::npos);

			/*replace the part between [] with '*' and call sub-model*/
			(*cmd).replace(found_l, found_r-found_l+1, "*");				//use * to replace the sub_command	
			init_model(find_model(sub_model), 0, sub_cmd);					//recursively call init_model			
			/*replace '*' with the output of the sub-model*/
			if(read_pipe(pipe_tag, &subst, ops))		
				(*cmd).replace((*cmd).find("*"), 1, subst);                   //replace * with the real value
			else {cout<<"cann't read tag or number from pipe.tmp file, please check"<<endl; exit(1);}
		}
	return 1;
}
int read_pipe(string pipe_tag, string* subst, string ops){
	char tmp[256];
	int num_good=0;
	int tag_good=0;
	string ops_tag;
	double num=0;
	double value=0;
	*subst="";

	/*get the digit from pipe.tmp file*/
	ifstream infile("pipe.tmp");	
	if(!infile) {cout<<"open pipe.tmp fail"<<endl; exit(1);}
	while(infile.getline(tmp,256)){								//get the specified output from pipe.tmp
		if(strstr(tmp,pipe_tag.c_str())!=NULL){					//find the line containing the specified output
			tag_good=1;
			break;		
		}
	}
	if(tag_good==0) return 0;
	char *pch=strstr(tmp,pipe_tag.c_str());
	int len=strlen(pipe_tag.c_str());
	for(int i=len; pch[i]!=0; i++){						
		if(!isdigit(pch[i]) && pch[i]!='.')	continue;
		else{
			while(isdigit(pch[i]) || pch[i]=='.'){		//find the nearest number(should be the value of the output) in the line
				(*subst)+=(pch[i]);	
				i++;						
			}
			num_good=1;
			break;										//break the read of digit
		}
	}
	
	/*do the extra operation specified by user*/
	stringstream ss_c(ops);	
	stringstream ss_v(*subst);
	stringstream ss_r;
	ss_v>>value;
	while(ss_c>>ops_tag>>num){
		if(ops_tag=="times"){ value=value*num; }
		else if(ops_tag=="divby"){if(value=0) {cout<<"divide 0"<<endl; exit(0);} value=num/value;}
		else if(ops_tag=="plus"){value=num+value;}
		else if(ops_tag=="pow"){value=pow(value,num);}
		else if(ops_tag=="") {}
		else {cout<<"please check your specified operation"<<endl; exit(1);}
	}
	ss_r<<value;
	*subst=ss_r.str();
	cout<<"The sub-called model output "<<*subst<<endl;
	return num_good;
}



/*for process and call a model
*/
int call_model(char** carg){
   
	char redirectin_info[256];
	char redirectout_info[256];
	bool in_flag=false;
	bool out_flag=false;
   
   for(int i=0; i<command_param_num-1;i++){					//check if there are "<" or ">" in command line.
  		if(carg[i]!=NULL){
   		if((strcmp(carg[i],"<")==0) &&  (carg[i+1]!=NULL)){

   			strcpy(redirectin_info, carg[i+1]);
			in_flag=true;
		}
		/*FIX ME: ">" actually not necessary, havn't use it yet*/
		else if((strcmp(carg[i],">")==0) &&  (carg[i+1]!=NULL)){

			strcpy(redirectout_info, carg[i+1]);
			out_flag=true;
		}
   		else if(((strcmp(carg[i],">")==0) ||(strcmp(carg[i],"<")==0))&& (carg[i+1]==NULL)){
   			cout<<"command format problem caused by redirect info, please check call_model function"<<endl; 
   			exit(1);
   		}
   		}
   	}
   	

   pid_t pID = fork();          //fork new process to call a model
   if (pID == 0){	
   
		char olddir[256];				//about working directory
		string newdir;
		string exe_name;
		newdir=string(carg[0]);	
		exe_name=newdir.substr(newdir.rfind("/")+1, -1);
		newdir=newdir.substr(0, newdir.rfind("/"));		
		getcwd(olddir, sizeof(olddir));
		chdir(newdir.c_str());			//change working directory to the model's directory
				   
   		if(in_flag==true){								//if there is "<" redirect the file content to stdin 		
   		   	const char* tmp_file_name=(const char*)redirectin_info ;
   		   	cout<<newdir<<" "<<tmp_file_name<<endl;
   			FILE *r_in=freopen(tmp_file_name, "r", stdin);
   			if(!r_in) {cout<<"open input stream file failed"<<endl; chdir(olddir); fclose(stdin); exit(1);}
   		}   
   
			chdir(olddir);
			FILE *f=freopen ("pipe.tmp","w",stdout);			//redirect recursive model's stdout to pipe.tmp file 
			chdir(newdir.c_str());
			if(!f) {cout<<"create pipe.tmp failed"; chdir(olddir); fclose(stdout); if(in_flag==true) fclose(stdin); exit(1);}


		int i=execv(exe_name.c_str(), carg);
		if(-1==i){
			cout<<"fail to execute the model, please check the parameter"<<endl;
			perror("error is");
			exit(1);
		}
		chdir(olddir);						//change working directory back to the interface directory
		fclose (stdout);
		if(in_flag==true)
			fclose (stdin);
	}
    else if (pID < 0){
        cout << "Failed to fork" << endl;
        exit(1);
    }
    else{
		int status;
		pid_t pid;
		pid=wait(&status);

//		cout<<pid<<endl;
    }
	release_mem_pp(carg);
	return 1;
}
/*help information
*/
int help(){
	cout<<"\n******** TOOL HELP MENU ********\n";
	cout<<"-h -- help"<<endl;
	cout<<"-m -- menu style guidence"<<endl;
	cout<<"-l -- list all models on the platform"<<endl;
	cout<<"-d -- implement input deduction - deduce more inputs from given ones"<<endl;
	cout<<"-n -- create a new input file based on config file - should do so whenever adding a new model to config file"<<endl;
	cout<<"-c <model number> -- directly call certain model"<<endl;
	cout<<"-c_dft <model number> <defult set number> -- call certain model with default value"<<endl;
	cout<<"-i <model number> -- show information of certain model"<<endl;
	cout<<"-s <output discreption> -- search for the models that may output what you want"<<endl;	
	cout<<"-input_check -- parse the input file to see what model you can use and what is absent for each model"<<endl;
	cout<<"-input_check <model number>-- parse the input file to see the inputs and outpus of the model you specified"<<endl;
return 1;
}
list<model_class>::iterator find_model(int k){
	list<model_class>::iterator	it;
	for(it=model_list.begin(); it!=model_list.end();it++){   //search the model in the link list
		if(k==it->num) break;
	}	
	if(it==model_list.end()) {cout<<"find no model as your input"<<endl; exit(0);}
	else
		return it;
 }
/*construct function for classes
*/
model_class::model_class(){
		num=0;
		comment="";
		guide="";
		for(int i=0; i<io_num; i++){
			input[i][0]="";
			input[i][1]="";
			input[i][2]="";
			input[i][3]="";
			output[i][0]="";
			output[i][1]="";
			dft[i]="";
			
		}
		for(int i=0; i<256; i++){
			dir[i]='\0';
		}
		ready=0;	
	}
input_class::input_class(){
	name="";
	content="";
}
