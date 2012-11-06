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




list<model_class> allcandidate[LENGTH];
list<string> uml_text_list;

int find_model_arc(list<string> *input_name_list, string outputs, int depth);
int find_candidate(string outputs, list<model_class> *candidate, list<string> *input_name_list);
bool check_dup(model_class a);
bool exist_in_list(string s, list<string> *input_name_list);
bool add_mark(string, int);
int reverse_find_model_arc(list<string> output_name_list, int depth);
int reverse_find_candidate(list<model_class> *candidate, list<string> *output_name_list);
int forward_find_model_arc(list<string> input_name_list, int depth);
int forward_find_candidate(list<model_class> *candidate, list<string> *input_name_list);
string without_unit(string name);			//when compire tags, get rid of unit after comma
int generate_uml_text(const char* filename);
string uml_text_format(list<model_class>::iterator it, list<model_class>::iterator it1);

int find_model_arc(list<string> *input_name_list, string outputs, int depth ){				//input/output based search
	int rt=1;
	for(int i=0; i<depth+1; i++){
		rt=find_candidate(outputs, &allcandidate[i], input_name_list);
		if(rt==0) break;
	}
	list<model_class>::iterator it;

/*	for(int i=0; i<depth+1; i++){
		if(!allcandidate[i].empty()){
			for(it=allcandidate[i].begin(); it!=allcandidate[i].end(); it++){				
				cout<<it->name<<" ";		
			}
			cout<<endl;			
		}	
	}*/
	generate_uml_text(uml_text_file);
	add_mark(outputs, depth);
	return rt;
}

	
int forward_find_model_arc(list<string> *input_name_list, int depth){			//input based search
	int rt=1;
	for(int i=0; i<depth+1; i++){
		rt=forward_find_candidate(&allcandidate[i], input_name_list);
		if(rt==0) break;
	}
	list<model_class>::iterator it;

/*	for(int i=0; i<depth+1; i++){
		if(!allcandidate[i].empty()){
			for(it=allcandidate[i].begin(); it!=allcandidate[i].end(); it++){				
				cout<<it->name<<" ";		
			}
			cout<<endl;			
		}	
	}*/
	generate_uml_text(uml_text_file);
	return rt;
}




int reverse_find_model_arc(list<string> *output_name_list, int depth){					//output based search

	int rt=1;
	for(int i=LENGTH-1; i>=LENGTH-depth-1; i--){	
		rt=reverse_find_candidate( &allcandidate[i], output_name_list);
		if(rt==0) break;
	}
	list<model_class>::iterator it;

/*	for(int i=0; i<depth+1; i++){
		if(!allcandidate[i].empty()){
			for(it=allcandidate[i].begin(); it!=allcandidate[i].end(); it++){				
				cout<<it->name<<" ";		
			}
			cout<<endl;			
		}	
	}*/

	generate_uml_text(uml_text_file);
	return rt;

}



int forward_find_candidate(list<model_class> *candidate, list<string> *input_name_list){
	int n;
	int flag;
	list<string> temp_input_name_list;
	list<string>::iterator iit;
	
	list<model_class>::iterator it;
	for(it=model_list.begin(); it!=model_list.end();it++){	
		if(check_dup(*it)) continue;
		flag=0;
		n=0;
		while(it->input[n][0]!=""){
			if(it->input[n][3]=="y" && (exist_in_list(it->input[n][0], input_name_list))){
				flag=1;
				break;
			}
			n++;		
		}
		if(1==flag){
			int m=0;
			while(it->output[m][0]!=""){
				temp_input_name_list.push_back(it->output[m][0]);	
				istringstream iss(it->output[m][2]);
				string sub;
				while(iss){
					iss>>sub;
					temp_input_name_list.push_back(sub);	
				}
				temp_input_name_list.push_back(it->output[m][0]);
				m++;
			}
			cout<<"model:"<<it->name<<endl;
			candidate->push_back(*it);					
		}	
	}
	if(!temp_input_name_list.empty()){
		for(iit=temp_input_name_list.begin(); iit!=temp_input_name_list.end(); iit++){	
			if(!exist_in_list(*iit, input_name_list))
				input_name_list->push_back(*iit);
		}
	}
	
	input_name_list->sort();
	input_name_list->unique();
	
	if(candidate->empty()) return 0;
	else return 1;	
}


int reverse_find_candidate( list<model_class> *candidate,list<string> *output_name_list){
	int n=0;
	int flag=0;
	list<string> temp_output_name_list;
	list<string>::iterator iit;
	
	list<model_class>::iterator it;
	for(it=model_list.begin(); it!=model_list.end();it++){	
		if(check_dup(*it)) continue;
		n=0; flag=0;
		while(it->output[n][0]!=""){
			if(exist_in_list(it->output[n][0], output_name_list)){
				flag=1;
			}
			istringstream iss(it->output[n][2]);
				string sub;
				while(iss){
					iss>>sub;
					if(exist_in_list(sub, output_name_list)){
						flag=1;
						break;
					
					}
				}
			if(flag==1) break;			
			n++;		
		}
		
		if(flag==1){
			cout<<"model:"<<it->name<<endl;
			candidate->push_back(*it);
			int m=0;
			while(it->input[m][0]!=""){
				temp_output_name_list.push_back(it->input[m][0]);
				m++;
			}
		}		
	
	
	}
	
	output_name_list->swap(temp_output_name_list);
	output_name_list->sort();
	output_name_list->unique();
	
		
	if(candidate->empty()) return 0;
	else return 1;	
}





int find_candidate(string outputs, list<model_class> *candidate, list<string> *input_name_list){
	int n;
	int flag;
	list<string> temp_input_name_list;
	list<string>::iterator iit;
	
	list<model_class>::iterator it;
	for(it=model_list.begin(); it!=model_list.end();it++){	
		if(check_dup(*it)) continue;
		flag=0;
		n=0;
		while(it->input[n][0]!=""){
			if(it->input[n][3]=="y" && (!exist_in_list(it->input[n][0], input_name_list))){
				flag=1;
				break;
			}
			n++;		
		}
		if(0==flag){
			int m=0;
			while(it->output[m][0]!=""){
				
				temp_input_name_list.push_back(it->output[m][0]);
				istringstream iss(it->output[m][2]);
				string sub;
				while(iss){
					iss>>sub;
					temp_input_name_list.push_back(sub);	
				}
				temp_input_name_list.push_back(it->output[m][0]);
				m++;
			}
			cout<<"model:"<<it->name<<endl;
			candidate->push_back(*it);					
		}	
	}
	if(!temp_input_name_list.empty()){
		for(iit=temp_input_name_list.begin(); iit!=temp_input_name_list.end(); iit++){	
			if(!exist_in_list(*iit, input_name_list))
				input_name_list->push_back(*iit);
		}
	}
	
	input_name_list->sort();
	input_name_list->unique();
	
	if(candidate->empty()) return 0;
	else return 1;	
}

bool check_dup(model_class a){
	list<model_class>::iterator it;
	
	for(int n=0; n<LENGTH; n++){
		if(!(allcandidate[n].empty())){
			for(it=allcandidate[n].begin(); it!=allcandidate[n].end();it++){		
				if(it->name==a.name)	return true;
			}
		}
	}
	return false;
}


string without_unit(string name){


	return name.substr(0, name.find(',', 0));

}



bool exist_in_list(string s, list<string> *input_name_list){	

	list<string>::iterator it;
	for(it=input_name_list->begin(); it!=input_name_list->end(); it++){	
		if(without_unit(*it)==without_unit(s)) return true;
		else continue;	
	}
	return false;
}


int generate_uml_text(const char* filename){

	ofstream FILE;
	 
	FILE.open(filename);
	
	int m,n,j,flag;			//flag to see if this model is the only one (no connection to others)
	list<model_class>::iterator it;
	list<model_class>::iterator it1;

	uml_text_list.clear();
	for(int i=0; i<LENGTH; i++){

		if(!allcandidate[i].empty()){ 
			for(it=allcandidate[i].begin(); it!=allcandidate[i].end(); it++){
				m=0;
				flag=0;
				while(it->output[m][0]!=""){
				
					for(j=i+1; j<LENGTH; j++){
						for(it1=allcandidate[j].begin(); it1!=allcandidate[j].end(); it1++){	
							n=0;
							while(it1->input[n][0]!=""){
								if(without_unit(it->output[m][0])==without_unit(it1->input[n][0])){
									
									uml_text_list.push_back(uml_text_format(it,it1));
									flag=1;
									break;

								}
								n++;
							}							



						}								

					}
				
					istringstream iss(it->output[m][2]);
					string sub;
					
					while(iss){	
						iss>>sub;
						
						for(j=i+1; j<LENGTH; j++){

							for(it1=allcandidate[j].begin(); it1!=allcandidate[j].end(); it1++){

								n=0;
								while(it1->input[n][0]!=""){

									if(without_unit(sub)==without_unit(it1->input[n][0])){
										flag=1;
										uml_text_list.push_back(uml_text_format(it,it1));
										break;

									}
									n++;
								}							



							}								

						}
			

					}
					m++;


				}	
			
			}
		}

	}

	uml_text_list.sort();
	uml_text_list.unique();
	list<string>::iterator lit;
	for(lit=uml_text_list.begin(); lit!= uml_text_list.end(); lit++){

		FILE<<*lit<<"\n";

	}

	FILE.close();


	return 1;
}


string uml_text_format(list<model_class>::iterator it, list<model_class>::iterator it1){

		string rt="["+it->name+"]"+"->"+"["+it1->name+"]";
	return rt;

}







bool add_mark(string name, int level){				//recursively add asterick mark
	int n;
	int m;
	list<model_class>::iterator it;
	if(level<0) return false;
	for(int i=level; i>=0; i--){
		if(!allcandidate[i].empty()){
			for(it=allcandidate[i].begin(); it!=allcandidate[i].end(); it++){	
				
				m=0;
				while(it->output[m][0]!=""){
				
					if(without_unit(it->output[m][0])==without_unit(name)){
						if(it->name.find("*")!=0)
							it->name="*"+it->name;
						n=0;
						while(it->input[n][0]!=""){
							add_mark(it->input[n][0], i-1);
							n++;
						}
					}
					else{
						istringstream iss(it->output[m][2]);
						string sub;
						while(iss){
							iss>>sub;
							if(without_unit(sub)==without_unit(name)){
								if(it->name.find("*")!=0)
									it->name="*"+it->name;
								n=0;
								while(it->input[n][0]!=""){
									add_mark(it->input[n][0], i-1);
									n++;
								}
							}
							
						}						
					}
					m++;
				}
			}
		}
	}	
	return true;
}

