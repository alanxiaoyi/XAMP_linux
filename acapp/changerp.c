/**
  * Please refer to License.txt for License information.
 */
#include "changerp.h"

/**
  * Generate the individual polynomail expression for each d
 */ 
int Work()
{
	int i;
	int readChk = 0;
	for (i=0;i<mAssoc;i++)
	{
		readChk = 0;
		mIndex=i;
		// minus 2 in order to remove the first and second accesses

		if (weightedAvg1[i]<53)
		{

			if (weightedAvg1[i]>31)
				weightedAvg1[i]=31;
			if (weightedAvg1[i]!=0)
			readChk = Read_coefficient(i,(int)(weightedAvg1[i]-1));
			if (readChk == -2)
				return -2;


		} else
			poly_coefficient[mIndex][0]=1;



	}

	return 1;
}

/**
  * Get coefficient values from file
 */ 
int Read_coefficient(int dx, int nx)
{
	int i,newnum;
	char strFile[200];
	FILE *fpmain;
	char sdx[200];
	char snx[200];
	char sassoc[200];
	char line[200];
	char *conv;
	sprintf(sdx,"%d",dx);
	sprintf(snx,"%d",nx);
	sprintf(sassoc,"%d",nassoc);


	switch (NRU)
	{
	case 1:
		chosenrpname = "NMRU4";
		strcpy(strFile,"./fine/NMRU4/d_");
		strcat(strFile, sdx);
		strcat(strFile, "/NMRU4");
		break;
	case 2:
		chosenrpname = "NMRU1";
		strcpy(strFile,"./fine/NMRU1/d_");
		strcat(strFile,sdx);
		strcat(strFile,"/NMRU1");
		break;
	case 3:
		chosenrpname = "LRUskw";
		strcpy(strFile,"./fine/Rand-LRUskw/d_");
		strcat(strFile,sdx);
		strcat(strFile,"/Rand-LRUskw");
		break;
	case 4:
		chosenrpname = "MRUskw";
		strcpy(strFile,"./fine/Rand-MRUskw/d_");
		strcat(strFile,sdx);
		strcat(strFile,"/Rand-MRUskw");
		break;
	default:
		if ((fpmain = fopen("rplist.log","r")) == NULL)
		{
			printf("ERROR: Please provide valid logfile rplist.log in working directory \n");
			exit(1);
		} else
		{
			while (fgets(line,200,fpmain) != NULL)
			{
				conv = strtok(line," ");
				newnum = atoi(conv);    
				if (newnum == NRU)
				{
					chosenrpname = conv;
					conv = strtok(NULL," ");
					conv = strtok(NULL," ");
					conv[strlen(conv)-1] = 0;
					strcpy(strFile,"./fine/");
					strcat(strFile,conv);
					strcat(strFile,"/d_");
					strcat(strFile,sdx);
					strcat(strFile,"/");
					strcat(strFile,conv);
				}


			}
			fclose(fpmain);
		}

		break;

	}


	strcat(strFile,":");
	strcat(strFile,sassoc);
	strcat(strFile,"_");
	strcat(strFile,sdx);
	strcat(strFile,"_");
	strcat(strFile,snx);
	strFile[strlen(strFile)] = 0;

	FILE *fp;
	fp=fopen(strFile,"r");
	if (fp==NULL)
	{
		printf("Missing Coefficient file: ");
		printf("%s\n",strFile);
		return -2;
	} else
	{

		for (i=0;i<MAX_NX;i++)
		{
			fscanf(fp,"%lf",&poly_coefficient[mIndex][i]);
		}   
		fclose(fp);
	}
	return 1;
}


/**
  * Generate the final polynomial expression before using the Newton-Raphson algorithm
 */ 
int Pre_coefficient()
{
	int i,j;
	double access_weight;

	//give the last with the previous one

	///////////////////////////////////////////////////////////////
	//copy the coefficient to stat_poly to calculte the miss rate for stack position
	///////////////////////////////////////////////////////////////

	for (i=0;i<mAssoc+1;i++)
		for (j=0;j<MAX_NX;j++)
			stat_poly_coefficient[i][j]=poly_coefficient[i][j];


	////////////////////////////////////
	//
	////////////////////////////////////

	//First. Weight the probability
	for (i=0;i<mAssoc;i++)
	{

		access_weight=(double)(stackDist1[i])/(double)(total_access);
		for (j=0;j<MAX_NX;j++)
			poly_coefficient[i][j]*=access_weight;
	}
	//Second, Merge the polynomial
	for (i=0;i<MAX_NX;i++)
	{
		for (j=0;j<mAssoc;j++)
			final_coefficient[i]+=poly_coefficient[j][i];
	}

	final_coefficient[0]+=(double)(stackDist1[mAssoc])/(double)(total_access);
	final_coefficient[1]-=1;
	return 1;
}

/**
  * Solve the final polynomial expression using the Newton Raphson method
 */ 

double Newton_Raphson(int nesting,double init)
{
	double numerate=0;
	double denominator=0;
	int i,j,k;
	double x=0;
	double x_cp=0;
	x_cp=x=init;
	k=0;
	while (1)
	{
		for (i=0;i<=nesting;i++)
			numerate+=final_coefficient[i]*pow(x,i);
		for (j=1;j<=nesting;j++)
			denominator+=final_coefficient[j]*j*pow(x,j-1);
		if (denominator==0)
			return 0;
		x=x-numerate/denominator;
		numerate=0;
		denominator=0;
		if (fabs(x-x_cp)<1e-6)
			break;
		x_cp=x;
		k++;
		if (k==20)
		{

			break;
		}
	}

	return x;
}

int print()
{
	int i;
	for (i=0;i<MAX_NX;i++)
		printf("%f \n",poly_coefficient[mIndex][i]);

	return 1;
}

/**
 * Report the LRU and Prediction results
*/
void Report(double miss_rate)
{
	printf("Prediction Result for %s\n",chosenrpname);
	printf("LRU: %f\n",(double)(lru_miss)/(double)(total_access));
	printf("Pred: %f\n",miss_rate);



}


