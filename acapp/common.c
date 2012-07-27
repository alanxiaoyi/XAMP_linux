/**
  * Please refer to License.txt for License information.
 */
#include "common.h"

/**
  * Initializes of the stack position replacement distribution
 */ 
void prob_init()
{
	int i,j;    
	int sum;
	double sumprob_check;
	FILE *fp;
	char line[2000];
	char *tokens;
	char *stprobtokens;
	char skipchk[1];
	sumprob_check = 0;
	sum=(1+nassoc)*nassoc/2;


	switch (NRU)
	{
	case 1:
		
		stack_location_prob = malloc(nassoc * sizeof(double));
		for (i=4;i<nassoc;i++)
			stack_location_prob[i]=(double)(1)/(double)(nassoc-4);
		for (j=0;j<4;j++)
			stack_location_prob[j]=0;
		strcpy(newrp_name,"NMRU4");
		break;
	case 2:
		
		stack_location_prob = malloc(nassoc * sizeof(double));
		for (i=1;i<nassoc;i++)
			stack_location_prob[i]=(double)(1)/(double)(nassoc-1);
		for (j=0;j<1;j++)
			stack_location_prob[j]=0;
		strcpy(newrp_name,"NMRU1");
		break;
	case 3:
		
		stack_location_prob = malloc(nassoc * sizeof(double));
		for (j=0;j<nassoc;j++)
			stack_location_prob[j]=(double)(j+1)/(double)(sum);
		strcpy(newrp_name,"Rand-LRUskw");
		break;

	case 4:
		
		stack_location_prob = malloc(nassoc * sizeof(double));
		for (j=0;j<nassoc;j++)
			stack_location_prob[nassoc-1-j]=(double)(j+1)/(double)(sum);
		strcpy(newrp_name,"Rand-MRUskw");
		break;
	case 5: 
		
		if ((fp = fopen("usr_rp.in","r")) == NULL)
		{
			printf("ERROR: Cannot find user input file usr_rp.in in working directory \n");
			exit(1);
		} else
		{


			while (fgets(line,2000,fp) != NULL)
			{
				strncpy(skipchk,line,1); 
				if (strstr(skipchk,"#") == NULL)
				{


					if (strstr(line,"NAME") != NULL)
					{
						tokens = strtok(line," ");
						tokens = strtok(NULL," ");
						strcpy(newrp_name, tokens);
						newrp_name[strlen(newrp_name)-1]= 0;
					}

					else if (strstr(line,"ASSOC") != NULL)
					{

						tokens = strtok(line," ");
						tokens = strtok(NULL," ");
						nassoc = atoi(tokens);
						stack_location_prob = malloc(nassoc * sizeof(double));

					}

					else if (strstr(line,"PROB") != NULL)
					{

						fgets(line,2000,fp);
						stprobtokens = strtok(line," ");
						for (i=0;i<nassoc;i++)
						{
							stack_location_prob[i] = atof(stprobtokens);
							stprobtokens = strtok(NULL," ");
						}
					}

				}

			} 

			for (i=0;i<nassoc;i++)
				sumprob_check+=stack_location_prob[i];
			if (sumprob_check != 1.0)
			{
				printf("ERROR: Sum of probability distributions should be equal to 1\n");
				printf("Please check usr_rp.in file\n");
				exit(1);


			}
			fclose(fp);
		}

	default: printf("\n");

	}
	return; 
}



/**
  * Reads in circular sequence data from the input CSQ file and initializes the circular sequence parameters and data structures
 */ 
void readCseq(char *cseq_outfile1)
{
	char line[2000];
	char *tokens;
	char *cseqtokens;
	char *stDistTokens;
	int i,j;
	int k;

	file1Available = 0;
	secondCseq = 0;
	sum_b1 = 0;
	sum_b2 = 0;
	prob_distinct_array = malloc(CSEQ_TABLE_SPACE*10*sizeof(double));
	for (i=0; i < CSEQ_TABLE_SPACE*10;i++)
		prob_distinct_array[i] = malloc((nassoc+1)*sizeof(double));
	i = 0;
	j = 0;  
	if ((fp1 = fopen(cseq_outfile1,"r")) == NULL)
	{
		printf("ERROR1: Please provide at least 1 valid CSQ file \n");
		exit(1);
	} else
	{
		while (fgets(line,2000,fp1) != NULL)
		{

			if (strstr(line,"#sets") != NULL)
			{
				tokens = strtok(line," ");
				tokens = strtok(NULL," ");
				nsets = atoi(tokens);

			}

			else if (strstr(line,"#assoc") != NULL)
			{
				tokens = strtok(line," ");
				tokens = strtok(NULL," ");
				nassoc = atoi(tokens);

			}

			else if (strstr(line,"#scaling_factor") != NULL)
			{
				tokens = strtok(line," ");
				tokens = strtok(NULL," ");
				nscale = atoi(tokens);


			} else if (strstr(line,"#block_size") != NULL)
			{
				tokens = strtok(line," ");
				tokens = strtok(NULL," ");
				nbsize = atoi(tokens);


			}

		}
		fclose(fp1);
	}
	dmax = nassoc*nscale; //PROFILE_ASSOC

	weightedAvg1 = malloc(dmax * sizeof(double));
	stackDist1 = malloc((dmax+1) * sizeof(int));

	if ((fp1 = fopen(cseq_outfile1,"r")) == NULL)
	{
		printf("ERROR: Error opening CSQ file\n");
	} else
	{
		while (fgets(line,2000,fp1) != NULL)
		{

			if (strstr(line,"#cseq") != NULL)
			{
				fgets(line,2000,fp1);
				cseqtokens = strtok(line," ");
				for (i=0;i<dmax;i++)
				{
					weightedAvg1[i] = atof(cseqtokens);
					cseqtokens = strtok(NULL," ");

				}
			} else if (strstr(line,"#stackDist") != NULL)
			{
				fgets(line,2000,fp1);
				stDistTokens = strtok(line," ");
				for (j=0;j<dmax+1;j++)
				{
					stackDist1[j] = atoi(stDistTokens);
					stDistTokens = strtok(NULL," ");

				}
			}
		}
		fclose(fp1); 
	}
	for (k=0; k < dmax+1;k++)
	{
		sum_b1+=stackDist1[k];
	}

	stat_err_prob_b1=stackDist1[dmax];

}

/**
  * Reads in two circular sequence data from the input CSQ file and initializes the circular sequence parameters and data structures for cache contentions
 */ 
void readCseqCon(char *cseq_outfile1, char *cseq_outfile2)
{
	char line[2000];
	char *tokens;
	char *cseqtokens;
	char *stDistTokens;
	int i,j;
	int k;

	file1Available = 0;
	secondCseq = 0;
	sum_b1 = 0;
	sum_b2 = 0;
	prob_distinct_array = malloc(CSEQ_TABLE_SPACE*10*sizeof(double));
	for (i=0; i < CSEQ_TABLE_SPACE*10;i++)
		prob_distinct_array[i] = malloc((nassoc+1)*sizeof(double));
	i = 0;
	j = 0;  

	if ((fp1 = fopen(cseq_outfile1,"r")) == NULL)
	{
		printf("ERROR: Please provide at 2 valid CSQ files \n");
		exit(1);
	} else
	{
		while (fgets(line,2000,fp1) != NULL)
		{

			if (strstr(line,"#sets") != NULL)
			{
				tokens = strtok(line," ");
				tokens = strtok(NULL," ");
				nsets = atoi(tokens);


			}

			else if (strstr(line,"#assoc") != NULL)
			{
				tokens = strtok(line," ");
				tokens = strtok(NULL," ");
				nassoc = atoi(tokens);


			}

			else if (strstr(line,"#scaling_factor") != NULL)
			{
				tokens = strtok(line," ");
				tokens = strtok(NULL," ");
				nscale = atoi(tokens);


			} else if (strstr(line,"#block_size") != NULL)
			{
				tokens = strtok(line," ");
				tokens = strtok(NULL," ");
				nbsize = atoi(tokens);


			}

		}
		fclose(fp1);
	}
	dmax = nassoc*nscale; //PROFILE_ASSOC
	weightedAvg1 = malloc(dmax * sizeof(double));
	stackDist1 = malloc((dmax+1) * sizeof(int));
	if ((fp1 = fopen(cseq_outfile1,"r")) == NULL)
	{
		printf("ERROR: Error opening file\n");
	} else
	{
		while (fgets(line,2000,fp1) != NULL)
		{

			if (strstr(line,"#cseq") != NULL)
			{


				fgets(line,2000,fp1);
				cseqtokens = strtok(line," ");
				for (i=0;i<dmax;i++)
				{
					weightedAvg1[i] = atof(cseqtokens);
					cseqtokens = strtok(NULL," ");


				}
			} else if (strstr(line,"#stackDist") != NULL)
			{
				fgets(line,2000,fp1);
				stDistTokens = strtok(line," ");
				for (j=0;j<dmax+1;j++)
				{
					stackDist1[j] = atoi(stDistTokens);
					stDistTokens = strtok(NULL," ");

				}
			}
		}
		fclose(fp1); 

		for (j=0; j < dmax + 1; j++)
		{
			if (j > nassoc)
				stackDist1[nassoc] += stackDist1[j];
		}

		for (j=nassoc+1; j < dmax + 1; j++)
			stackDist1[j] = 0;

	}

	//second csq file
	weightedAvg2 = malloc(dmax * sizeof(double));
	stackDist2 = malloc((dmax+1) * sizeof(int));
	if (cseq_outfile2 != NULL)
	{
		if ((fp2 = fopen(cseq_outfile2,"r")) == NULL)
		{
			printf("ERROR: 2nd CSQ file is not valid\n");
			exit(1);
		} else
		{
			secondCseq = 1;
			i = 0; j = 0; cseqtokens = NULL; stDistTokens = NULL;


			while (fgets(line,2000,fp2) != NULL)
			{

				if (strstr(line,"#cseq") != NULL)
				{


					fgets(line,2000,fp2);
					cseqtokens = strtok(line," ");

					for (i=0;i<dmax;i++)
					{
						weightedAvg2[i] = atof(cseqtokens);
						cseqtokens = strtok(NULL," ");

					}
				} else if (strstr(line,"#stackDist") != NULL)
				{
					fgets(line,2000,fp2);
					stDistTokens = strtok(line," ");
					for (j=0;j<dmax+1;j++)
					{
						stackDist2[j] = atoi(stDistTokens);
						stDistTokens = strtok(NULL," ");

					}
				}
			}
			fclose(fp2);

			for (j=0; j < dmax + 1; j++)
			{
				if (j > nassoc)
				{
					stackDist2[nassoc] += stackDist2[j];

				}
			}
			for (j=nassoc+1; j < dmax + 1; j++)
				stackDist2[j] = 0;

		}
		for (k=0; k < dmax+1;k++)
			sum_b2+=stackDist2[k];    
		stat_err_prob_b2=stackDist2[nassoc];
	}
	for (k=0; k < dmax+1;k++)
	{
		sum_b1+=stackDist1[k];
	}

	stat_err_prob_b1=stackDist1[nassoc];
}   


/**
  * Initializes the circular sequence parameters and data structures required to make a prediction from a specified replacement policy
 */ 
void readCseq2(int vNRU)
{
	int i,j;
	NRU = vNRU;
	mAssoc = nassoc * 3;
	poly_coefficient = malloc((mAssoc+1)*sizeof(double));
	stat_poly_coefficient = malloc((mAssoc+1)*sizeof(double));
	final_coefficient = malloc(MAX_NX*sizeof(double));
	for (i=0;i<mAssoc+1;i++)
		poly_coefficient[i]= malloc(MAX_NX * sizeof(double));;
	for (i=0;i<mAssoc+1;i++)
		stat_poly_coefficient[i]=malloc(MAX_NX * sizeof(double));
	for (i=0;i<mAssoc+1;i++)
		for (j=0;j<MAX_NX;j++)
			poly_coefficient[i][j]=0;

	
	for (i=mAssoc+1;i<dmax+1;i++)
		stackDist1[mAssoc]+=stackDist1[i];
	total_access=0;
	for (i=0;i<mAssoc+1;i++)
		total_access+=stackDist1[i];
	lru_miss=0;
	for (i=nassoc;i<mAssoc+1;i++)
	{

		lru_miss+=stackDist1[i];
	}
	readFast = 1;

}

/**
  * Initializes the circular sequence parameters and data structures required to create a user-specified replacement policy
 */ 
void readCseq3(int vNRU)
{
	int i,j;
	NRU = vNRU;
	prob_init();
	mAssoc = nassoc * 3;
	poly_coefficient = malloc((mAssoc+1)*sizeof(double));
	stat_poly_coefficient = malloc((mAssoc+1)*sizeof(double));
	for (i=0;i<mAssoc+1;i++)
		poly_coefficient[i]= malloc(MAX_NX * sizeof(double));
	for (i=0;i<mAssoc+1;i++)
		stat_poly_coefficient[i]=malloc(MAX_NX * sizeof(double));
	for (i=0;i<mAssoc+1;i++)
		for (j=0;j<MAX_NX;j++)
			poly_coefficient[i][j]=0;
}

/**
  * Frees the allocated storage space used to store the circular sequence and its associated parameters
 */
void freeSpace(){
	int i;
	free(stackDist1);   
	free(weightedAvg1); 
	free(stack_location_prob);
	if (secondCseq == 1)
	{
		free(weightedAvg2);
		free(stackDist2);
	}
	if (readFast == 1)
	{

		for (i=0;i<mAssoc+1;i++)
			free(poly_coefficient[i]);
		for (i=0;i<mAssoc+1;i++)
			free(stat_poly_coefficient[i]);
		free(poly_coefficient);
		free(stat_poly_coefficient);
		free(final_coefficient);
		for (i=0; i < CSEQ_TABLE_SPACE*10;i++)
			free(prob_distinct_array[i]);
		free(prob_distinct_array);

	}


}

/**
  * Frees the allocated storage space used to create a replacement policy or make a prediction
 */
void freeSpace2(){
	int i;
	free(stack_location_prob);
	for (i=0;i<mAssoc+1;i++)
		free(poly_coefficient[i]);
	for (i=0;i<mAssoc+1;i++)
		free(stat_poly_coefficient[i]);
	free(poly_coefficient);
	free(stat_poly_coefficient);
	free(final_coefficient);

}


void printCseq(){
	printf("The CSQ file is generated using the following cache parameters:	\n");
	printf("\tSets: %d\n",nsets);
	printf("\tAssociativity: %d\n",nassoc);
	printf("\tBlock size: %d\n",nbsize);

}
