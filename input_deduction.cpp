#include "interface.h"


#define num_of_deduction 2        //modify this if add new deduction function
#define num_of_argus 10        

using namespace std;

extern int create_input_file(list<input_class> );
string i2i[num_of_deduction][num_of_argus];		
string func_pointer0(string*);
string func_pointer1(string*);      //add new deduction function declaration here


//define a function pointer array to save all deduction functions
string (*func_pointer[num_of_deduction])(string*)={
	func_pointer0,
	func_pointer1					// add new deduction function here: save into the function pointer array
};              						


int input_deduction(){
	string argu[num_of_argus];	
	//init the i2i and argu array
	for(int j=0;j<num_of_argus;j++){
		for(int i=0; i<num_of_deduction; i++){
			i2i[i][j]="";
		}
		argu[j]="";
	}
	
/*add source and destination of new deduction.
For example, i2i[0][0] is the destination(power law index a) 
of the deduction. This deduction needs one source: csqfile path.
Put destination in i2i[n][0], sources follow behind*/ 

	i2i[0][0]="powerlaw_index";			//dest
	i2i[0][1]="csqfile1";				//source

	
	i2i[1][0]="CPI_infinitL2,i";		//dest
	i2i[1][1]="CPIpreL2";			
	i2i[1][2]="apiL2";
	i2i[1][3]="hit_latency";




/*************************************************************/
/*you don't need to change any code between the sign*/
/*************************************************************/
	
	list<input_class>::iterator it;
	list<input_class>::iterator itc;
	int derivesomething=0;
	int meetornot;

	
	for(int i=0; i<num_of_deduction;i++){
		meetornot=0;
		for(int j=1; j<num_of_argus; j++){
			if(i2i[i][j]!=""){
				for(it=input_list.begin(); it!=input_list.end();it++){		
					if(it->name==i2i[i][j] && it->content!=""){
						argu[j]=it->content;
						meetornot++;
					}
				}
			}
			else meetornot++;
		}

		if(meetornot<num_of_argus-1) continue;								//shows that the deduction function cannot be called due to lack of arguments
		
		for(it=input_list.begin(); it!=input_list.end();it++){		
			if(it->name==i2i[i][0] && it->content==""){						//cout<<it->name<<endl;							
				it->content=func_pointer[i](argu);				
				cout<<"using value of input \"";
				for(int j=1; j<num_of_argus; j++){
					if(i2i[i][j]!="")
						cout<<i2i[i][j]<<" ";
					else break;
				}
				cout<<"\" the tool derives out \""<<i2i[i][0]<<"\""<<endl;
				derivesomething=1;	
				break;
			}
		}
	}	

	if(derivesomething==0) cout<<"derive out nothing!"<<endl;
	else {
		create_input_file(input_list);
		cout<<"Please check the input file to find out new values derived from existed ones!"<<endl;		
	}
	return derivesomething;
}
/*************************************************************/
/*you don't need to change any code between the sign*/
/*************************************************************/




/*Below, you should implement your deduction functions.
from argu[1] are the sources you designated in the 2-d array.
argu[0] is the destination
*/
string func_pointer0(string* argu){ 		//deduction function 1 for csq to power law index value.
	int N=0;
	int assoc;
	int scale_f;
	int nassoc;
	int num_point;
	int start_point;
	float a;
	string A;
	int n=0;	
	char tmp[256];
	string tmpstr;
	ifstream csq;
	csq.open(argu[1].c_str());
	if(!csq)  cout<<"open csq file failed"<<endl;	
	while(csq.getline(tmp,256)){    //istream.getline
		tmpstr=tmp;	
		if(tmpstr.find("#assoc")!=-1){
			stringstream ss(tmpstr);
			ss>>tmpstr;	
			ss>>assoc;
		}
		else if(tmpstr.find("#scaling_factor")!=-1){
			stringstream ss(tmpstr);
			ss>>tmpstr;
			ss>>scale_f;
			break;
		}
	}
	nassoc=assoc*scale_f;
	num_point=nassoc;
	start_point=0;
	int *stack=new int[nassoc+1];
	long *total_misses=new long[nassoc];
	float *missrate=new float[nassoc];
	float* c_log=new float[nassoc];
	float* m_log=new float[nassoc];
	
	for(int i=0; i<nassoc; i++){    //init
		total_misses[i]=0;
		missrate[i]=0;
		c_log[i]=0;
		m_log[i]=0;
		stack[i]=0;
	}
	stack[nassoc]=0;

	while(csq.getline(tmp,256)){    //istream.getline
		tmpstr=tmp;	
		if(tmpstr.find("#stackDist")!=-1){ 
			csq.getline(tmp,256);			
			tmpstr=tmp;
			string iotmp;
			stringstream ss(tmpstr);
			n=0;
			while(n<nassoc+1){
				ss>>stack[n];
				n++;
			}
		}
	}	
	n=0;
	while(n<nassoc+1){
		N=stack[n]+N;
		n++;	
	}
	for(int i=0; i<nassoc;i++){
		int j=i+1;

		while(j<=nassoc){
			total_misses[i]=stack[j]+total_misses[i];	
			j++;
		}
		missrate[i]=(double)total_misses[i]/N;
		m_log[i]=log(missrate[i]);
		c_log[i]=log(i+1);
	}
//for least square regression	
	float x_square_sum=0;
	float x_sum_square=0;
	float xy_sum=0;
	float xy=0;
	float sumx=0;
	float sumy=0;
	
	for(int i=start_point; i<num_point;i++){	
		x_square_sum+=c_log[i]*c_log[i];
		x_sum_square+=c_log[i];
		xy_sum+=c_log[i]*m_log[i];
		sumx+=c_log[i];
		sumy+=m_log[i];	
	}

	x_sum_square=x_sum_square*x_sum_square/(num_point-start_point);
	xy=sumx*sumy/(num_point-start_point);
	
	a=-(xy_sum-xy)/(x_square_sum-x_sum_square);

	char at[10];
	gcvt(a,4,at);
	A=at;
	csq.close();
	return A;
}

/*deduction function 2 for demo example
*/
string func_pointer1(string* argu){
	float result;
	string result_s;
	char result_c[10];
	result = atof(argu[1].c_str())+atof(argu[2].c_str())*atof(argu[3].c_str());
	gcvt(result, 4, result_c);
	result_s=string(result_c);
return result_s;

}
