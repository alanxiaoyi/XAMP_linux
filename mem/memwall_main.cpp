#include "math.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>

using namespace std;

int *P;  //number of CEA for cores (equal to number of cores)
int *N;  //number of CEA of die area
float S1;
float B_rate=1;
float *B;
float D=1;
int Gen;
float a;
float F=1;
int k=0;

float newton(float , int, float);

int help(){
	cout<<"Enter parameters as \" <#Gen> <N0> <P0> <a> <F> <B_rate> <D>\" "<<endl;
	cout<<"#Gen: how many generation you want to predict"<<endl;
	cout<<"N0: number of CEAs of base line setting //integer"<<endl;
	cout<<"P0: number of CEAs(equal to core number) for core of base line setting //integer"<<endl;
	cout<<"a: power law index"<<endl;
	cout<<"F: facter for indirect Techniques"<<endl;
	cout<<"B_rate: bandwidth increase rate per generation"<<endl;
	cout<<"D: factor for direct techniques"<<endl;
	return 1;
}

int main(int argc, char * argv[]){
	
	if(argc!=8){help(); exit(0);}
	Gen=atoi(argv[1]);
	P=new int[Gen];
	N=new int[Gen];
    B=new float[Gen];	
	B[0]=1;
	N[0]=atoi(argv[2]);
	P[0]=atoi(argv[3]);
	a=atof(argv[4]);
	F=atof(argv[5]);
	B_rate=atof(argv[6]);
	D=atof(argv[7]);
	
	
	S1=(N[0]-P[0])/P[0];

	cout<<"Generation0: "<<P[0]<<endl;
	for(int i=0; i<Gen; i++){
		N[i]=N[0]*pow(2,i);	
		B[i]=B[0]*pow(B_rate,i);
//		cout<<"B"<<i<<" :"<<B[i]<<endl;
	}
	
	for(int i=1; i<Gen; i++){
		k=0;
		P[i]= ceil(newton(P[0]*(i+1),N[i],B[i]));
		cout<<"Generation"<<i<<": "<<P[i]<<endl;
	}

	return 1;
}

float newton(float x, int N2, float B){   //use newton method to get the solution
	float f;
	float df;
//	cout<<"x:"<<x<<endl;
	k++;
	if(k==20) {cout<<"newton method too many iteration"<<endl; exit(0);	}				//
	f=(1+a)*log(x)-a*log(N2-x)+a*log(S1)-log(P[0])-a*log(F)-log(B)+log(D);
//	cout<<"f:"<<f<<endl; 
	df=(1+a)/x+a/(N2-x);
//	cout<<"df:"<<df<<endl; 
	if((x-f/df)/x>0.9 && (x-f/df)/x<1.1) return (x-f/df);
	else 
	return (newton(x-f/df, N2, B));
}