#include "interface.h"




list<model_class> allcandidate[LENGTH];

int find_candidate(string outputs, list<model_class> *candidate, list<string> *input_name_list);
bool check_dup(model_class a);
bool input_exist(string s, list<string> *input_name_list);


int find_model_arc(list<string> *input_name_list, string outputs){
	int rt=1;
	for(int i=0; i<LENGTH; i++){
		rt=find_candidate(outputs, &allcandidate[i], input_name_list);
		if(rt==2 || rt==0) break;
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
			if(input_exist(it->input[n][0], input_name_list));
			else {
				flag=1;
				break;
			}
			n++;		
		}
		if(0==flag){
			int m=0;
			while(it->output[m][0]!=""){
				if(it->output[m][0]==outputs){
					
					candidate->push_back(*it);
					
					return 2;					
				}
				else{	
					istringstream iss(it->output[m][2]);
					string sub;
					while(iss){
						iss>>sub;
						temp_input_name_list.push_back(sub);	
					}
					temp_input_name_list.push_back(it->output[m][0]);
				}
				m++;
			}
			cout<<"model:"<<it->name<<endl;
			candidate->push_back(*it);					
		}	
	}
	if(!temp_input_name_list.empty()){
		for(iit=temp_input_name_list.begin(); iit!=temp_input_name_list.end(); iit++){		
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

bool input_exist(string s, list<string> *input_name_list){	

	list<string>::iterator it;
	for(it=input_name_list->begin(); it!=input_name_list->end(); it++){	
		if((*it)==s) return true;
		else continue;	
	}
	return false;
}