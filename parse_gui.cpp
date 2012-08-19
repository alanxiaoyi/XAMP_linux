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
#include "tinyxml.h"
#include "interface.h"

using namespace std;


/*for sorting input_list
*/
bool compare_input(input_class a, input_class b) {								
	bool flag1=(((a.name).c_str())[0]<((b.name).c_str())[0]);
	bool flag2=(((a.name).c_str())[1]<((b.name).c_str())[1]);
	
	if(((a.name).c_str())[0]!=((b.name).c_str())[0]) return flag1;
	else return flag2;
}

int create_input_file(list<input_class> input_list){
	
	list<input_class>::iterator its;	
	cout<<"creating input file according to config file"<<endl;		
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );		
	doc.LinkEndChild(decl);
	for(its=input_list.begin(); its!=input_list.end();its++){
		TiXmlElement * input=new TiXmlElement("input");
		input->SetAttribute("name", (its->name).c_str());
		input->SetAttribute("value", (its->content).c_str());
		input->SetAttribute("description", (its->comment).c_str());
		doc.LinkEndChild(input);
	}
	doc.SaveFile("input.xml");

}


/*parse configuration file function
*/
int parse_config(const char* a, bool createornot){
	cout<<"parsing the config file..."<<endl;
	int num=0;
	int n;
	
	TiXmlDocument doc;
	const char* configxml="config.xml";
	if(doc.LoadFile(configxml)){
		model_list.clear();
		TiXmlHandle docHandle(&doc);
		TiXmlElement* model=docHandle.FirstChild("model_list").FirstChild("model").ToElement();
		for(model; model; model=model->NextSiblingElement("model")){
			num++;
			model_class newmodel;
			model_list.push_back(newmodel);				
			model_list.back().name=(string)(model->Attribute("name"));
			model_list.back().num=num;
			
			for(TiXmlElement* elemt=model->FirstChildElement(); elemt; elemt= elemt->NextSiblingElement()){
				try{
					if(strcmp(elemt->Value(),"directory")==0){
						strcpy(model_list.back().dir, elemt->Attribute("name"));
					}
					else if(strcmp(elemt->Value(),"model_description")==0){
						if(elemt->GetText())
							model_list.back().comment=(string)(elemt->GetText());
					}
					else if(strcmp(elemt->Value(),"user_guide")==0){
						if(elemt->GetText())
							model_list.back().guide=(string)(elemt->GetText());
					}
					else if(strcmp(elemt->Value(),"inputs")==0){
						n=0;
						for(TiXmlElement * child=elemt->FirstChildElement(); child; child=child->NextSiblingElement()){
							if(child->Attribute("name")!="")
								model_list.back().input[n][0]=(string)(child->Attribute("name"));
							else {cout<<"empty name in xml!"<<endl; exit(1);}
							model_list.back().input[n][1]=(string)(child->Attribute("default"));
							model_list.back().input[n][2]=(string)(child->Attribute("description"))+", from model ["+model->Attribute("ID")+"]; ";
							model_list.back().input[n][3]=(string)(child->Attribute("primary"));
							n++;
						}
					}
					else if(strcmp(elemt->Value(),"outputs")==0){
						n=0;
						for(TiXmlElement * child=elemt->FirstChildElement(); child; child=child->NextSiblingElement()){
							if(child->Attribute("name")!="")
								model_list.back().output[n][0]=(string)(child->Attribute("name"));
							else {cout<<"empty name in xml!"<<endl; exit(1);}
							n++;
						}
					}
					else if(strcmp(elemt->Value(),"defaults")==0){
						n=0;
						for(TiXmlElement * child=elemt->FirstChildElement(); child; child=child->NextSiblingElement()){
							if(child->Attribute("command")!="")
								model_list.back().dft[n]=(string)(child->Attribute("command"));
							else {cout<<"empty command in xml!"<<endl; exit(1);}
							n++;
						}
					}
				}
				catch(...){cout<<"XML formmat problem may happen, check parse_config or parse_gui file for more info!"<<endl;}
			}
		}		
	}
	else {
		cout<<"loading XML file failed "<<endl;
		cout<<doc.ErrorDesc()<<endl;
		cout<<"row: "<<doc.ErrorRow ()<<" column: "<<doc.ErrorCol ()<<endl;
		return 0;

	}
	

	if(createornot){
	//save the input into list	
		list<model_class>::iterator it;
		list<input_class>::iterator its;
		list<input_class> input_list_t;							//notice its a temprory list only for creating the input file. Another input_list defined as global
		
		for(it=model_list.begin(); it!=model_list.end();it++){   //iterate the model in the link list
			int n=0;
			while(it->input[n][0]!=""){
				input_class newinput_t; 
				newinput_t.name=it->input[n][0];
				if(it->input[n][1]!=""){
					newinput_t.content=it->input[n][1];
				}
				if(it->input[n][2]!=""){
					newinput_t.comment=it->input[n][2];
				}
				for(its=input_list_t.begin(); its!=input_list_t.end();its++){       //search for duplicated one, then combine the comments and erase it
					if(its->name==it->input[n][0]){
						if(its->content!="" && it->input[n][1]=="")					//default value
							newinput_t.content=its->content;
						newinput_t.comment=its->comment+" "+newinput_t.comment;	
						its=input_list_t.erase(its);
					}		
				}			
				
				string::iterator sit;
				for(sit=newinput_t.comment.begin();sit!=newinput_t.comment.end(); sit++){   //search for endline simble and replace them
					if(*sit=='\r' || *sit=='\n') newinput_t.comment.replace(sit, sit+1, "");
				}			
				input_list_t.push_back(newinput_t);
				n++;
			}
		}
		input_list_t.sort(compare_input);
			
		create_input_file(input_list_t);				 //create the "input" file	

	}
	cout<<"*parsing over!*"<<endl;
	return 1;
}

/*parse input file function
*/
int parse_input_file(const char* a){
	input_list.clear();    										//create a new input list every time parsing input_file
	cout<<"parsing input file now ..."<<endl;
	list<model_class>::iterator it;
	TiXmlDocument doc;

	if(doc.LoadFile(a)){	
		TiXmlHandle docHandle(&doc);
		TiXmlElement* inputelemt=docHandle.FirstChildElement().ToElement();
		for(inputelemt; inputelemt; inputelemt=inputelemt->NextSiblingElement("input")){
			input_class newinput;
			newinput.name=(string)(inputelemt->Attribute("name"));
			newinput.content=(string)(inputelemt->Attribute("value"));
			for(it=model_list.begin(); it!=model_list.end();it++){   //search the model in the link list and then fill in the content
				int n=0;
				while(it->input[n][0]!=""){
					if(newinput.name==it->input[n][0])
						it->input[n][1]=newinput.content;	
						n++;
				}
				
			}
			newinput.comment=(string)(inputelemt->Attribute("description"));
			input_list.push_back(newinput);							
		}	
	}
	else {
		cout<<"loading XML file failed "<<endl;
		cout<<doc.ErrorDesc()<<endl;
		cout<<"row: "<<doc.ErrorRow()<<" column: "<<doc.ErrorCol()<<endl;
		exit(1);

	}
	check_ready();
	return 1;
	
}

/*check if one model is ready to use(have all inputs), and set the ready bit
*/
 
int check_ready(){
	int n;
	int flag;
	list<model_class>::iterator it;
	for(it=model_list.begin(); it!=model_list.end();it++){	
		n=0;
		flag=0;
		while(it->input[n][0]!=""){
			if(it->input[n][1]=="") {
				flag=1;
				break;
			}
			else n++;
		}
		if(0==flag) it->ready=1;
	}
}



