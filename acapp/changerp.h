#define CSEQ_TABLE_SPACE 200
#define MAX_NX 60
#include "common.h"

char *chosenrpname;
int fileExists; 
int Work();
int Read_coefficient(int dx, int nx);
int Pre_coefficient();
double Newton_Raphson(int nesting,double init);		
void Report(double miss_rate);	
		

