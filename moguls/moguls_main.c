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
#include "math.h"
#include <stdio.h>
#include <iostream>
#define max_cache_level 4  //starting level + 3 extra level
#include <stdlib.h>

using namespace std;

float rou;    //factor that influence the power
float P;	//power
int BM;		//memory bandwidth
int n;		//cache level
double BP[max_cache_level]; //bandwidth of each level cache
double C[max_cache_level];	//size of each level cache

int help(){
	cout<<"Enter parameters as \" <BRc> <BM> <p> <C0> \" "<<endl;
	cout<<"BRc is the core bandwidth requirement, however, in the model, we use the bandwidth requirement of the first initial level cache instead"<<endl;
	cout<<"BM  is the off-chip memory bandwidth "<<endl;
	cout<<"p is the factor that determined by tech and process, which influence the power consumption"<<endl;
	cout<<"C0 is the initial level cache size"<<endl;
return 1;
}

int print_result(){
	cout<<"cache level is: "<<n<<endl;
	cout<<"power is: "<<P<<endl;
	for(int i=1; i<=n; i++){
		cout<<"level: "<<i;
		cout<<" bandwidth: "<<BP[i];
		cout<<" size: "<<C[i]<<endl;
	}
return 1;

}

int main(int argc, char *argv[]){

	if(argc!=5){
		help();
		exit(0);
	}

	for(int i=0; i<max_cache_level;i++){
		BP[i]=0;
		C[i]=0;
	}

	BP[0]=atoi(argv[1]);
	BM=atoi(argv[2]);
	rou=atof(argv[3]);
	C[0]=atoi(argv[4]);

	double BRc;
	BRc=BP[0];

	double Bratio=BRc/BM;
	
	if(Bratio<1) {									//if BRC<BM, there is no need of extra cache hierarchy
		n=0;	
		P=0;
	}	
	else if((log(BRc/BM))>(max_cache_level-1)) {	//we set our cache hierarchy cannot be larger than max cache level
		n=max_cache_level-1;
		double ntmp=1.0/(double)n;
		P=n*rou*sqrt(C[0])*BRc*pow(Bratio,ntmp);		
	}
	
	else {										
		n=ceil(log(BRc/BM));	
		double ntmp=1.0/(double)n;
		P=n*rou*sqrt(C[0])*BRc*pow(Bratio,ntmp);
	}
		
	
	
	switch(n){
		case 0:
			break;
		case 1:
			BP[1]=BRc;
			C[1]=C[0]*pow(BRc/BM, 2);
			break;
		case 2:
			BP[1]=BRc;
			BP[2]=pow(BRc*BM, 0.5);
			C[1]=C[0]*pow(BRc/BP[2],2);
			C[2]=C[0]*pow(BRc/BM,2);
			break;
		case 3:
			BP[1]=BRc;
			double Btmp;
			Btmp=pow(BRc/BM, 0.3333333);
			BP[2]=BRc/Btmp;
			BP[3]=BP[2]/Btmp;
			C[1]=C[0]*pow(BRc/BP[2],2);
			C[2]=C[0]*pow(BRc/BP[3],2);
			C[3]=C[0]*pow(BRc/BM,2);
			break;		
		default:
			cout<<"level exception!"<<endl;
			break; 
	
	}
	
	print_result();
	return 1;	
}