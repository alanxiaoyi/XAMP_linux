/**
  * Please refer to License.txt for License information.
 */
#include "contention.h"

void printDetails(char *cseq1, char *cseq2){
	
	printCseq();
	printf("\t\t\t******** RESULTS ********\n");
	printf("\n                     \t %s \t %s ",cseq1,cseq2);
	printf("\nAccesses:\t\t %d \t\t\t %d", sum_b1, sum_b2);
	printf("\nPredicted miss rate: \t %f \t\t\t %f",(double)(stat_err_prob_b1)/(double)(sum_b1),(double)(stat_err_prob_b2)/(double)(sum_b2));
	printf("\n%ld, %ld, %ld, %ld", stat_err_prob_b1, sum_b1, stat_err_prob_b2, sum_b2);
	printf("\nOriginal miss rate: \t %f \t\t\t %f\n",(double)(stackDist1[nassoc])/(double)(sum_b1),(double)(stackDist2[nassoc])/(double)(sum_b2));
}

/**
  * This function sums the probability of containing less than k distinct accesses within a circular sequence
 */
double prob_left(int k,int index)
{
	int i;
	int sum_left=0;
	//calculate the p(K-), 
	if (index==BENCH_MARK1)
	{
		for (i=0;i<k;i++)
			sum_left+=stackDist1[i];
		return(double)(sum_left)/(double)(sum_b1);
	} else if (index==BENCH_MARK2)
	{
		for (i=0;i<k;i++)
			sum_left+=stackDist2[i];
		return(double)(sum_left)/(double)(sum_b2); 
	}

	return -1;
}


/**
 * Please refer to the core algorithm described in [2].  Please see README.txt
 */
double prob_seq_distinct_iterate(int k,int m,int n,int index,int N)
{
	double ret;

	if (m<n)
	{
		ret= 0;
	} else if (k==N)
	{
		ret= 1;
	} else if (k==0)
	{
		return prob_seq_distinct_iterate(1, m-1, n-1, index, N);
	} else
	{
		ret=prob_left(k,index)*prob_seq_distinct_iterate(k, m-1, n, index, N)+(1-prob_left(k,index))*prob_seq_distinct_iterate(k+1, m-1, n-1, index, N);

	}

	if (k==1)
	{
		//when rollback here. current m and n are m-1 and n-1
		prob_distinct_array[m+1][n+1]=ret;
	}
	return ret;

}

/**
  * This function predicts the contention results for each benchmark
 */
int prob(int index)
{

	int i;
	int j;
	int k;
	int nx,nx_tmp,ny;
	double ret;

	//clean
	for (i=0;i<CSEQ_TABLE_SPACE;i++)
		for (j=0;j<nassoc+1;j++)
			prob_distinct_array[i][j]=0;

	for (k=0;k<CSEQ_TABLE_SPACE;k++)
	{
		prob_distinct_array[k][1]=1;
	}



	for (i=nassoc;i>0;i--)
	{
		nx=0;
		nx_tmp=0;
		ny=0;

		////////////////////////////////////////////////////////
		//			calculate the E(nx) and E(ny)
		//		for cseq(d,nx) the access intervals  are "nx-1"
		/////////////////////////////////// /////////////////
		if (index==BENCH_MARK1)
		{
			//reminder that the first column of the cseq is the line num which can be ignored
			nx= weightedAvg1[i];
			ny= (int)((double)(nx*sum_b2)/(double)(sum_b1));

			if (ny>60)
				ny=60;
			//////////////////////////////////////
			//then calculate the P(seq(y))
			//
			//then calculate the extra miss number
			////////////////////////////////////////

			ret=prob_seq_distinct_iterate(0, ny, i,BENCH_MARK2, i);
			stat_err_prob_b1+=(int)(stackDist1[nassoc-i]*prob_distinct_array[ny][i]);


		} else if (index==BENCH_MARK2)
		{



			nx=weightedAvg2[i];
			ny=(int)((double)(nx*sum_b1)/(double)(sum_b2));

			if (ny>60)
				ny=60;

			ret=prob_seq_distinct_iterate(0, ny, i,BENCH_MARK1, i);


			stat_err_prob_b2+=(int)(stackDist2[nassoc-i]*prob_distinct_array[ny][i]);



		}

	}

	return 1;
}



