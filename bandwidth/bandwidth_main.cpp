#include "math.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <cstring>

using namespace std;

string file="./input";	//this is the ralitive path to the working directory, i.e. the directory of interface, not the directory of the model
float *M;							//threads' L2 cache miss rate
float *tm;							//threads' average L2 miss penalty(#cycles)
float *A;							//threads' L2 cache access frequency
int p=0;							//niumber of cores(threads)
float *CPIalone;					//the IPC achieved by one thread runnning alone on a CMP system
float *CPIil2;						//Threas' CPI assuming infinite L2 cache size 
float *bn;							//Fraction of off-chip bandwidth assigned to thread i (natural partition)
float *bo;							//Fraction of off-chip bandwidth assigned to thread i (optimal partition)
float BW;							//Total off-chip bandwidth
int K;								//cache block size(Bytes)
float WSo=0;						//weighted speed up when optimal partition the bandwidth
float WSn=0;						//weighted speed up when naturally partition the bandwidth
float fclk;
int calculate(int );

/*parse the input file
*/
int read_input(){
	ifstream input;
	input.open(file.c_str());
	if(!input)  cout<<"open input file failed"<<endl;	
	char tmp[256];
	string tmpstr;
	string tmpstr1;
	while(input.getline(tmp,256)){	
		tmpstr=tmp;
		if(tmpstr.find("number of cores")!=-1){
			input.getline(tmp,256);
			p=atoi(tmp);
			M=new float[p];
			A=new float [p];
			CPIalone=new float[p];
			CPIil2=new float[p];
			tm=new float[p];
			bn= new float[p];
			bo= new float[p];
			
			for(int i=0; i<p; i++){
				M[i]=0;
				A[i]=0;
				CPIalone[i]=0;
				CPIil2[i]=0;
				tm[i]=0;
				bn[i]=0;
				bo[i]=0;			
			}
		}
		
		else if(tmpstr.find("#Mi")!=-1 || tmpstr.find("#Ai")!=-1 || tmpstr.find("#CPIalone")!=-1 || tmpstr.find("#CPI,infinitL2")!=-1 || tmpstr.find("#tm")!=-1){
			if(!p) {cout<<"please check the format of input file!"<<endl; return 0;}
			input.getline(tmp,256);
			tmpstr1=tmp;
			stringstream ss(tmpstr1);	
			for(int i=0; i<p; i++){
				ss>>tmpstr1;		
				if(tmpstr.find("#Mi")!=-1)
					M[i]=atof(tmpstr1.c_str());
				else if (tmpstr.find("#Ai")!=-1 )
					A[i]=atof(tmpstr1.c_str());
				else if (tmpstr.find("#CPIalone")!=-1 )
					CPIalone[i]=atof(tmpstr1.c_str());
				else if (tmpstr.find("#CPI,infinitL2")!=-1 )
					CPIil2[i]=atof(tmpstr1.c_str());
				else if (tmpstr.find("#tm")!=-1 )
					tm[i]=atof(tmpstr1.c_str());
			}
		}
		
		else if(tmpstr.find("#fclk")!=-1){
			input.getline(tmp,256);
			fclk=atoi(tmp);		
		}
		else if(tmpstr.find("#B")!=-1){
			input.getline(tmp,256);
			BW=atof(tmp);		
		}
		else if(tmpstr.find("#K")!=-1){
			input.getline(tmp,256);
			K=atoi(tmp);		
		}
	
	}

	return 1;
}

/*help information
*/
int help(){
	cout<<"You can use three mode, but all should complete input file fist"<<endl;
	cout<<"-input -- use the parameter in inputfile to generate the bandwidth partition information"<<endl;
	cout<<"-Bandwidth <minimum> <maximum> -- change the main memory bandwidth from minimum to maximum, and output all the bandwidth partition information"<<endl;
	cout<<"-MAratio <M0> <A0> -- input one thread's miss rate and memory access information, then the tool change the miss frequency ratio of two threads by changing the miss rate of one thread. Output all bandwidth partition information"<<endl;
return 1;
}



int main(int argc, char *argv[]){
	
			read_input();		
			if(argc==2 && strcmp(argv[1], "-input")==0){				//output information based only on the input file
				calculate(1);
			}
			
			/*according to the observation 4 from the paper
			*/
			else if(argc==4 && strcmp(argv[1], "-Bandwidth")==0){	//output information based on input file and extra bandwidth you enter
				float BWs=atof(argv[2]);					//minimum bandwidth:start
				float BWe=atof(argv[3]);					//maximum bandwidth: end
				if(BWe-BWs<=0) {cout<<"you should enter minimum Bandwidth as first parameter, then the maximum Bandwidth: <minimum> <maximum>"<<endl; exit(0);}
				for(BW=BWs; BW<=BWe; BW+=(BWe-BWs)/10){		//the tool vary the bandwidth between your specified minimum and maximum, step is (maxmum-minimum)/10
					calculate(2);
				}					
			}
			
			/*according to the observation 5 from the paper
			*/
			else if(argc==4 && strcmp(argv[1], "-MAratio")==0){		//The tool vary one thread's missrate atomatically. Number of threads is fixed to 2
				M[0]=atof(argv[2]); A[0]=atof(argv[3]);				//user should put one thread's missrate and access frequency
				A[1]=A[0];											//the second thread's access frequency is the same to the first one but miss rate will change automatically
				p=2;
				float max;
				if(M[0]>=0.1) max=1;
				else max=M[0]*10;									//the second thread's access frequency change between 0.1*M[0] to 10*M[0](or 1). 
				for(M[1]=M[0]/10; M[1]<=max; M[1]+=((max-M[0]/10)/10)){
					calculate(3);			
				}
			}	
			else{
				help();
			
			}
			return 1;			
}

/*the calculation process according to the model
*/
int calculate(int flag){
				float MA_ps=0;
				float MAC_ps=0;
				for(int i=0; i<p; i++){
					MA_ps+=M[i]*A[i];
					MAC_ps+=pow(M[i]*A[i],0.6666666)*pow(CPIalone[i]/CPIil2[i],0.3333333);						
				}			
	//			cout<<"MA: "<<MA_ps<<" "<<MAC_ps<<endl;
				for(int i=0; i<p; i++){
					bn[i]=M[i]*A[i]/MA_ps;
					bo[i]=pow(M[i]*A[i],0.6666666)*pow(CPIalone[i]/CPIil2[i], 0.3333333)/MAC_ps;		
				}
				
				for(int i=0; i<p; i++){
					WSn+=CPIalone[i]*(1-M[i]*A[i]*tm[i]/(fclk*1000)-(M[i]*A[i]*M[i]*A[i]*K*K)/(bn[i]*bn[i]*BW*BW*1000000))/CPIil2[i];
					WSo+=CPIalone[i]*(1-M[i]*A[i]*tm[i]/(fclk*1000)-(M[i]*A[i]*M[i]*A[i]*K*K)/(bo[i]*bo[i]*BW*BW*1000000))/CPIil2[i];									
				}
				
				/*print out different information according to different requirement
				*/
				if(flag==1){
					for(int i=0; i<p; i++){				
						cout<<"thread"<<i<<": bnature:"<<bn[i]<<" boptimal:"<<bo[i]<<endl;	
					}

					cout<<"weighted speed up nature:"<<WSn<<" weighted speed up optimal:"<<WSo<<endl;
					cout<<"optimal-nature is:"<<WSo-WSn<<endl;
				}
				
				else if(flag==2){
					cout<<"When Bandwidth is:"<<BW<<endl;
					for(int i=0; i<p; i++){				
						cout<<"\tthread"<<i<<": bnature:"<<bn[i]<<" boptimal:"<<bo[i]<<endl;	
					}
					cout<<"\tweighted speed up nature:"<<WSn<<" weighted speed up optimal:"<<WSo<<endl;
					cout<<"\toptimal-nature is:"<<WSo-WSn<<endl;								
				}
				else if(flag==3){
					cout<<"When ratio is:"<<(M[1]*A[1])/(M[0]*A[0])<<endl;
					for(int i=0; i<p; i++){				
						cout<<"\tthread"<<i<<": bnature:"<<bn[i]<<" boptimal:"<<bo[i]<<endl;	
					}
					cout<<"\tweighted speed up nature:"<<WSn<<" weighted speed up optimal:"<<WSo<<endl;
					cout<<"\toptimal-nature is:"<<WSo-WSn<<endl;								
				}
				return 1;	
}