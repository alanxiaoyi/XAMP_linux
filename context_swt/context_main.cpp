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
#include <sstream>
#include <fstream>
#include <stdlib.h>

using namespace std;
string file;
int read_csq();
int *stack;						//according to each counter in CSQ or stack-distance profiling file
float *preuse;					//probability that a block in the ith stack position to be reused in the next cache access
float *pstay;					//probability that the holes stay the same prosition
float *pcondmiss;				//conditional probability of miss
float *pshift;					//probability that the holes shift by one position
float ***p;						//array to save the probability of each state
int N=0;    				 	//mem access num
int NpS=0;  					//mem access per set
int A=8;   	 					//# of assoc   
int nassoc;
int sets; 
int calculate_p();
int shift_amount=6;
float totmiss=0;
float prob_state(int , int, int);

/*init all the dynamic array
*/
int init(){
	for(int i=0; i<A+1; i++)
		for(int j=0; j<A+1; j++)
			p[i][j]= new float[NpS];
	for(int i=0; i<A+1; i++){
		pstay[i]=0;
		pshift[i]=0;
		pcondmiss[i]=0;
		for(int j=0; j<A+1; j++)
			for(int k=0; k<NpS; k++)
				p[i][j][k]=-1;
	}
	for(int j=0; j<nassoc+1; j++){	
		preuse[j]=0;
	}

}
int help(){
	cout<<"Enter parameters as \" A, shift_amount, csqfile path \" "<<endl;
	cout<<"A is the number of associativity, shift_amount is the abstracted amount of cache perturbation"<<endl;
return 1;
}



int main(int argc, char *argv[]){
	
	if(argc==4){
		A=atoi(argv[1]);
		shift_amount=atoi(argv[2]);
		if(shift_amount>A) shift_amount=A;
		file=argv[3];
	}
	else{
		help();
		exit(0);	
	}
	/*after getting A, init arrays here
	*/
	pstay= new float[A+1];	
	pshift=new float[A+1];
	pcondmiss=new float[A+1];
	p=new float**[A+1];
	for(int i=0; i<A+1; i++)
		p[i]=new float*[A+1];	
	read_csq();
	init();
	calculate_p();
	
	for(int c=A-shift_amount; c<=A; c++)
		for(int h=1; h<=A+1; h++)
			for(int n=0; n<=NpS-1; n++){
				totmiss+=prob_state(c,h,n)*pcondmiss[c];
			}
	float natmiss=NpS*pcondmiss[A];
	if(shift_amount==0) totmiss=natmiss;
	float csmiss=totmiss-natmiss;
	cout<<"associtivity number "<<A<<endl;
	cout<<"shift amount "<<shift_amount<<endl;
	cout<<"nature miss per set: "<<natmiss<<endl;
	cout<<"totle miss per set: "<<totmiss<<endl;
	cout<<"context switch miss per set: "<<csmiss<<endl;
}
/*readin the csq file
*/
int read_csq(){
	int assoc;
	int scale_f;
	int n=0;	
	char tmp[256];
	string tmpstr;
	ifstream csq;
	csq.open(file.c_str());
	if(!csq)  cout<<"open csq file failed"<<endl;	
	while(csq.getline(tmp,256)){    //istream.getline
		tmpstr=tmp;	
		if(tmpstr.find("#assoc")!=-1){
			stringstream ss(tmpstr);
			ss>>tmpstr;	
			ss>>assoc;
		}
		else if(tmpstr.find("#sets")!=-1){
			stringstream ss(tmpstr);
			ss>>tmpstr;
			ss>>sets;	
		}
		else if(tmpstr.find("#scaling_factor")!=-1){
			stringstream ss(tmpstr);
			ss>>tmpstr;
			ss>>scale_f;
			break;
		}
	}
	nassoc=assoc*scale_f;
	stack=new int[nassoc+1];
	preuse=new float[nassoc+1];
	
	for(int i=0; i<nassoc; i++){    
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
	NpS=(int)((float)N/(float)sets+0.5);
	if(NpS==0) {
		NpS=1;
		cout<<"access number may be too small"<<endl;
	}
csq.close();
	return 1;
}

/*calculate all probability used in markov model
*/
int calculate_p(){
	for(int i=0; i<nassoc+1; i++){
		preuse[i]=(float)stack[i]/N;		
	}
	for (int i=0; i<=A; i++){
		for(int j=0; j<i; j++){
			pstay[i]+=preuse[j];
		}
		if(i>0){
			for(int k=i-1; k<nassoc+1; k++)
				pshift[i]+=preuse[k];
		}
		else pshift[i]=0;
//			cout<<pshift[i]<<endl;
	}	
	pstay[A]=1;

	for(int i=0; i<=A; i++){
		for(int j=i; j<nassoc+1;j++)
			pcondmiss[i]+=preuse[j];
		
	}
	return 1;
}

/*recursivly iterate markov model
*/
float prob_state(int c, int h, int n){
	if(h>c+1) return 0;
	if(h>=1 && c>=0 && n>=0){
		if(p[c][h-1][n]!=-1)
			return p[c][h-1][n];
		else{
		float ret=0;
		if(c==A-shift_amount && h==1 && n==0){
			return 1;
		}
		else if(c>=A-shift_amount && h==c+1 && n >0)
			ret=prob_state(c, h, n-1)*pstay[h-1]+prob_state(c,h-1,n-1)*pshift[h-1]+prob_state(c-1,h-1,n-1)*pshift[h-1];
		else if(c>=A-shift_amount && h<c+1 && n>0)
			ret= prob_state(c,h,n-1)*pstay[h-1]+prob_state(c,h-1,n-1)*pshift[h-1];
		else {
			p[c][h-1][n]=0;
			return 0;	
		}
		p[c][h-1][n]=ret;				//save every result into a 3-d array to speed up the recursion
		return ret;
		}
	}
	else return 0;
}
