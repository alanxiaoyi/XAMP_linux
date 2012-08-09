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
#include <stdlib.h>

using namespace std;
int help(){
	cout<<"Enter parameters as \" <fparn> <fparc> <pctn> <n_threads> <p> <a> <b> , note a, b, p is optional. P for asymmetric core, a, b for ACS \" "<<endl;
return 1;
}

int main(int argc, char *argv[]){

if(argc>8 || argc<=1){
	help();
	exit(0);
}

float fparn;
float fparc;
float pctn;
float fseq;
float pcs;
int n;
float p;
float pcf;
float a, b;
float t;

// symmetric cores
a=0;
b=1;
p=1;
fparn=atof(argv[1]);
fparc=atof(argv[2]);
pctn=atof(argv[3]);
n=atoi(argv[4]);

if (argc>5) p=atoi(argv[5]);
if (argc>6) a=atoi(argv[6]);
if (argc>7) b=atoi(argv[7]);


fseq = 1-fparn-fparc;
pcs=fparc/(fparn+fparc);
pcf = pcs*pctn;


float part1, part2, part3;

part1=fseq/p;
part2=fparc*(a*pcf+(1-b)*(1-pcf))/p+(1-a)*fparc*pcf+(fparc*b*(1-pcf)+fparn)/(n-1+p);
part3=fparc*(a*pctn+(1-b)*(1-pctn))/p+(1-a)*fparc*pctn+(fparc*b*(1-pctn)+fparn)/(2*(n-1+p));

t=part1+max(part2, part3);

cout<<"execution time correlation to "<<t<<endl;

return 1;


}