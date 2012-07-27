/**
  * Please refer to License.txt for License information.
 */
#include "common.h"

/**
  * Calculates the original miss rate and miss rate for range of specified associativities 
  */
void sumAssoc(char Astr1[], char Astr2[]){
	int i,j;
	int new_misses = 0;
	int orig_misses = 0;
	int A1,A2;
	double miss_rate = 0.0;
	double newmiss_rate = 0.0;

	A1 = atoi(Astr1);
	A2 = atoi(Astr2);

	if ((A1 < 1) || (A1 > dmax))
	{
		printf("\nERROR: Please enter a valid integer less than %d and greater than 0\n",dmax);
		exit(1);
	}
	if (A1 > A2)
	{
		printf("\nERROR: Please enter a valid range of integers (from smallest to largest) \n");
		exit(1);

	}
	for (i=nassoc;i<=dmax;i++)
	{

		orig_misses += stackDist1[i];
	}
	miss_rate = (double)orig_misses/sum_b1;
	printCseq();
	printf("Original miss rate: %f\n",miss_rate);
	

	for (j = A1; j <= A2; j++)
	{
		new_misses = 0;
		for (i=j;i<=dmax;i++)
		{
			new_misses += stackDist1[i];
			
		}   
		newmiss_rate = (double)new_misses/sum_b1;

		printf("When A=%d, Miss rate is:  %f\n",j, newmiss_rate);
	}
}

