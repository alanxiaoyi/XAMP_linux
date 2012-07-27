#include "math.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <cstring>

using namespace std;

string file="./input";


float CPIpreL2;//(CPI in the core and L1s)
float apiL2;//(L2 accesses per inst) 
float lL2;  //(L2 hit latency)
int n;  //(number of cores)
float AC; //(total area of the chip for cores and cache, in CEAs) //1 CEA equal to 1MB cache area
float Ac; //(area of each core in CEAs)
float m1MBL2; //(L2 miss rate for a 1MB L2)
float a; //(power law index)
float E; //(Data sharing impact on miss rates)
float TM; //(memory access penalty)
float B; //(memory bandwidth in GB/s)
int bL2; //(L2 cache block size in bytes)
float f; //processor freq
float fs; //fraction cannot be parallelized
float acct_1=0;		//access time of 1M cache
float acct_n=0;		//access time of nM cache (consume n CEAs area)


int help(){
	cout<<"Please comlete the inputfile in the model directory first.  "<<endl;
	cout<<"-input --the tool will output the CPI and IPC information based on the parameter you put in the inputfile"<<endl;
	cout<<"-cvc --the tool will generate the Chip IPC vs the Core area in CEAs. It provides information of optimal core to cache ratio"<<endl;
return 1;
}

int read_input(){
	ifstream input;
	input.open(file.c_str());
	if(!input)  cout<<"open input file failed"<<endl;	
	char tmp[256];
	string tmpstr;
	while(input.getline(tmp,256)){	
		tmpstr=tmp;
		if(tmpstr.find("CPIpreL2")!=-1){
			input.getline(tmp,256);
			CPIpreL2=atof(tmp);		
		}		
		else if(tmpstr.find("apiL2")!=-1){
			input.getline(tmp,256);
			apiL2=atof(tmp);		
		}
		else if(tmpstr.find("lL2")!=-1){
			input.getline(tmp,256);
			lL2=atof(tmp);		
		}
		else if(tmpstr.find("number of cores")!=-1){
			input.getline(tmp,256);
			n=atoi(tmp);		
		}
		else if(tmpstr.find("total area of the chip")!=-1){
			input.getline(tmp,256);
			AC=atof(tmp);		
		}
		else if(tmpstr.find("area of each core")!=-1){
			input.getline(tmp,256);
			Ac=atof(tmp);		
		}		
		else if(tmpstr.find("m1MBL2")!=-1){
			input.getline(tmp,256);
			m1MBL2=atof(tmp);		
		}
		else if(tmpstr.find("power law index")!=-1){
			input.getline(tmp,256);
			a=atof(tmp);		
		}
		else if(tmpstr.find("TM")!=-1){
			input.getline(tmp,256);
			TM=atof(tmp);		
		}
		else if(tmpstr.find("memory bandwidth")!=-1){
			input.getline(tmp,256);
			B=atof(tmp);		
		}
		else if(tmpstr.find("bL2")!=-1){
			input.getline(tmp,256);
			bL2=atoi(tmp);		
		}
		else if(tmpstr.find("E(n)")!=-1){
			input.getline(tmp,256);
			E=atof(tmp);		
		}
		else if(tmpstr.find("processor frequency")!=-1){
			input.getline(tmp,256);
			f=atof(tmp);		
		}
		else if(tmpstr.find("fraction of program")!=-1){
			input.getline(tmp,256);
			fs=atof(tmp);		
		}
		else if(tmpstr.find("1M_acctime")!=-1){
			input.getline(tmp,256);
			acct_1=atof(tmp);		
		}
		else if(tmpstr.find("nM_acctime")!=-1){
			input.getline(tmp,256);
			acct_n=atof(tmp);		
//			cout<<"acct"<<acct_n<<endl;
		}
	
	}

return 1;
}


int main(int argc, char* argv[]){
		
	read_input();
	
	float CPIc;			//Cycles per Instruction at a core
	float IPCC;			//instructions per Cycle of the chip
	float mpiL2;        //L2 misses per instruction;
	float a1, a2, a3;   //coefficient

	
	if(argc>=2 && strcmp(argv[1], "-input")==0){
		
		if(argc>2){
			string subst="";
			for( int i=2; i<argc; i++){
				subst=subst+string(argv[i])+" ";		
			}
			
			if(subst.find("-AC")!=string::npos){
				string tmp=subst.substr(subst.find("-AC"), string::npos);
				stringstream tmp_s(tmp);
				tmp_s>>tmp>>tmp;
				AC=atof(tmp.c_str());
			}
			if(subst.find("-n")!=string::npos){
				string tmp=subst.substr(subst.find("-n"), string::npos);
				stringstream tmp_s(tmp);
				tmp_s>>tmp>>tmp;
				n=atoi(tmp.c_str());
			}
			if(subst.find("-lL2")!=string::npos){
				string tmp=subst.substr(subst.find("-lL2"), string::npos);
				stringstream tmp_s(tmp);
				tmp_s>>tmp>>tmp;
				lL2=atof(tmp.c_str());
			}
		}
		
		a1=2*B*B;
		mpiL2=apiL2*m1MBL2*pow((AC-Ac*n)/n, -a)*E;
		a2=(2*B*f*n*mpiL2*bL2+(CPIpreL2+apiL2*lL2+mpiL2*TM)*2*B*B);
		a3=(CPIpreL2+apiL2*lL2+mpiL2*TM)*(2*B*f*n*mpiL2*bL2)-mpiL2*mpiL2*f*f*n*n*bL2;
		//the quadratic equation is a1*CPI^2-a2CPi+a3=0;

		CPIc=(a2+pow(a2*a2-4*a1*a3, 0.5))/(2*a1);
		
		cout<<"CPI per core is: "<<CPIc<<endl;
		IPCC=n/((1+fs*(n-1))*CPIc);
		cout<<"chip IPC is : "<<IPCC<<endl;
	}
	
	else if(argc==2 && strcmp(argv[1], "-cvc")==0){	
		a1=2*B*B;
		for(n=1; n<AC; n+=10){
			cout<<"total CEA number is: "<<AC<<endl;
			cout<<"number of Cores(CEAs for Core): "<<n<<"\t";
			mpiL2=apiL2*m1MBL2*pow((AC-Ac*n)/n, -a)*E;
			if(acct_n!=0 && acct_1!=0)
				lL2=ceil(((256-n)*(acct_n-acct_1)/(AC-1)+acct_1)/(1.0/f));		//predict iL2
//			cout<<"ll2:"<<lL2<<endl;
			a2=(2*B*f*n*mpiL2*bL2+(CPIpreL2+apiL2*lL2+mpiL2*TM)*2*B*B);
			a3=(CPIpreL2+apiL2*lL2+mpiL2*TM)*(2*B*f*n*mpiL2*bL2)-mpiL2*mpiL2*f*f*n*n*bL2;		
			CPIc=(a2+pow(a2*a2-4*a1*a3, 0.5))/(2*a1);		
			IPCC=n/((1+fs*(n-1))*CPIc);
			cout<<"chip IPC is: "<<IPCC<<endl;	
		}
	}
	else {help();}	
	return 1;
}
