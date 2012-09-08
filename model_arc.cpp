#include "interface.h"




list<model_class> allcandidate[LENGTH];

int find_candidate(string outputs, list<model_class> *candidate, list<string> *input_name_list);
bool check_dup(model_class a);
bool exist_in_list(string s, list<string> *input_name_list);
bool add_mark(string, int);
int reverse_find_model_arc(list<string> output_name_list);
int reverse_find_candidate(list<model_class> *candidate, list<string> *input_name_list);

int find_model_arc(list<string> *input_name_list, string outputs){
	int rt=1;
	for(int i=0; i<LENGTH; i++){
		rt=find_candidate(outputs, &allcandidate[i], input_name_list);
		if(rt==0) break;
	}
	list<model_class>::iterator it;

	for(int i=0; i<LENGTH; i++){
		if(!allcandidate[i].empty()){
			for(it=allcandidate[i].begin(); it!=allcandidate[i].end(); it++){				
				cout<<it->name<<" ";		
			}
			cout<<endl;			
		}	
	}
	add_mark(outputs, 3);
	return rt;
}


int reverse_find_model_arc(list<string> *output_name_list){

	int rt=1;
	for(int i=0; i<LENGTH; i++){
		rt=reverse_find_candidate( &allcandidate[i], output_name_list);
		if(rt==0) break;
	}
	list<model_class>::iterator it;

	for(int i=0; i<LENGTH; i++){
		if(!allcandidate[i].empty()){
			for(it=allcandidate[i].begin(); it!=allcandidate[i].end(); it++){				
				cout<<it->name<<" ";		
			}
			cout<<endl;			
		}	
	}
	return rt;

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

bool exist_in_list(string s, list<string> *input_name_list){	

	list<string>::iterator it;
	for(it=input_name_list->begin(); it!=input_name_list->end(); it++){	
		if((*it)==s) return true;
		else continue;	
	}
	return false;
}


bool add_mark(string name, int level){
	int n;
	int m;
	list<model_class>::iterator it;
	if(level<0) return false;
	for(int i=level; i>=0; i--){
		if(!allcandidate[i].empty()){
			for(it=allcandidate[i].begin(); it!=allcandidate[i].end(); it++){	
				
				m=0;
				while(it->output[m][0]!=""){
				
					if(it->output[m][0]==name){
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
							if(sub==name){
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

