#include "interface.h"

using namespace std;

int construct_menu(){
	 string s_line="-------------------";
	 list<model_class>::iterator itm;
	 int l1;
	 int l2;
	 int l3;
	 int m;
	 int flag;
	 while(1){
		 l1=0;
		 l2=0;
		 l3=0;
		 flag=0;
		 m=0;
		 cout<<s_line<<endl;
		 cout<<"1. What do you want? "<<endl;
		 cout<<"2. What do you have? "<<endl;
		 cin>>l1;
		if(cin.fail()){
			cin.clear();
			cin.ignore();		
		}
		cin.ignore();
         cout<<s_line<<endl;
		switch(l1)	{
			case 1:  
				cout<<"Choose the number of model you want to use, or enter something else to go back"<<endl;
				for(itm=model_list.begin(); itm!=model_list.end();itm++){   //search the model in the link list
					cout<<itm->num<<". "<<itm->name<<endl;
				}				
				cin>>l2;
				if(cin.fail()){
					cin.clear();
					cin.ignore();		
				}
				cin.ignore();
				cout<<s_line<<endl;
				if(l2<=(model_list.back().num) && l2>=1)
					return l2;
				else continue;
				
			case 2:
				cout<<"please put the information you have into the inputfile"<<endl;
				cout<<"press ENTER to continue"<<endl;
				cin.get();
				cout<<s_line<<endl;
				parse_input_file(input_file_name);
				cout<<"implementing input deduction"<<endl;
				if(input_deduction()) parse_input_file(input_file_name);
				cout<<"below(if any) is the model you can use with your input:"<<endl;
				for(itm=model_list.begin(); itm!=model_list.end();itm++){
					if(itm->ready==1){
						cout<<itm->num<<". "<<itm->name<<endl;
						flag=1;
					}
				}

				cout<<"*choose one model from the list above or enter 0 to check the absent input for each model"<<endl;
				cin>>l2;
				if(cin.fail()){
					cin.clear();
					cin.ignore();		
				}
				cin.ignore();
				cout<<s_line<<endl;
				if(l2)	{
					if(l2<=(model_list.back().num)&&l2>=1)
						return l2;
					else continue;
				}
				
				else{
					for(itm=model_list.begin(); itm!=model_list.end();itm++){
						int n=0;
						if(itm->ready==0){
							cout<<itm->num<<". "<<itm->name<<endl;
							cout<<"this model needs additional value of primary inputs: \n";
							while(itm->input[n][0]!=""){
								if(itm->input[n][1]=="" && itm->input[n][3]=="y")
									cout<<itm->input[n][0]<<", ";
								n++;
							}
						cout<<"\n---------------\n";
						}
					}
				cout<<"*press ENTER to go back"<<endl;
				cin.get();
				cout<<s_line<<endl;
				}				
			default: continue;
		}
	}
	return -1;
}