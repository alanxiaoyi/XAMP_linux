/**
  * Please refer to License.txt for License information.
 */

#include "createrp.h"


/**
  * The implementaition of markov process presented in the paper[1] in readme.
 */
void PMiss(int d, int nx, int p, int expon,double cur_coefficient, int d_init,int nx_init)
{

	double Prob_distinct;
	double Prob_miss_shift;
	double P_pre=0;
	double P_suf=0;
	int i;
	int d_pass=0;

	d_pass=d_init-d;//here d means the remaining distinct access;
	
	///////////////////////////////////////////////
	//for a miss there is a probability that it would not shift
	////////////////////////////////////////////////
	for (i=0;i<p;i++)
		P_pre+=stack_location_prob[i];
	for (i=p+1;i<nassoc;i++)
		P_suf+=stack_location_prob[i];
	Prob_miss_shift=P_suf/(P_suf+P_pre);

	

	if (nx==0)
		return;

	Prob_distinct=(double)d/(double)nx;	  //prob of this access is a distinct access

	if (nx<d)
		return;

	if (d<0)
		return;

	if (d==0&&d_pass==p)
		return;

	if (cur_coefficient==0)
		return;

	if ((nx_init>18)&&((fabs(cur_coefficient)<1e-7&&expon>1)||(fabs(cur_coefficient)<1e-8&&expon==1)))
		return;

	if (d_pass==p)
	{
		if (p==(nassoc-1)) //last position in the stack
		{
			//if(expon==0&&stack_location_prob[p]!=0)
			//theLog<<stack_location_prob[p]*Prob_distinct*cur_coefficient<<endi;
			poly_coefficient[mIndex][expon]+=stack_location_prob[p]*Prob_distinct*cur_coefficient;
			PMiss(d-1,nx-1,p,expon,Prob_distinct*(1.0-stack_location_prob[p])*cur_coefficient,d_init,nx_init);
			PMiss(d,nx-1,p,expon,(1.0-Prob_distinct)*cur_coefficient,d_init,nx_init);               
		} else
		{
			poly_coefficient[mIndex][expon+1]+=stack_location_prob[p]*Prob_distinct*cur_coefficient;
			PMiss(d-1,nx-1,p+1,expon,Prob_distinct*cur_coefficient,d_init,nx_init);
			PMiss(d-1,nx-1,p+1,expon+1,Prob_distinct*((1.0-stack_location_prob[p])*Prob_miss_shift-1.0)*cur_coefficient,d_init,nx_init);
			PMiss(d-1,nx-1,p,expon+1,Prob_distinct*(1.0-stack_location_prob[p])*(1.0-Prob_miss_shift)*cur_coefficient,d_init,nx_init);
			PMiss(d,nx-1,p,expon,(1.0-Prob_distinct)*cur_coefficient,d_init,nx_init);
		}

	} else
	{
		if (p==(nassoc-1))
		{

			poly_coefficient[mIndex][expon]+=stack_location_prob[p]*Prob_distinct*cur_coefficient;
			poly_coefficient[mIndex][expon+1]+=stack_location_prob[p]*(1.0-Prob_distinct)*cur_coefficient;
			PMiss(d-1,nx-1,p,expon,Prob_distinct*(1.0-stack_location_prob[p])*cur_coefficient,d_init,nx_init);
			PMiss(d,nx-1,p,expon+1,-1*stack_location_prob[p]*(1.0-Prob_distinct)*cur_coefficient,d_init,nx_init);               
			PMiss(d,nx-1,p,expon,(1.0-Prob_distinct)*cur_coefficient,d_init,nx_init);               


		} else
		{
			poly_coefficient[mIndex][expon+1]+=stack_location_prob[p]*cur_coefficient;
			PMiss(d-1,nx-1,p+1,expon,Prob_distinct*cur_coefficient,d_init,nx_init);
			PMiss(d-1,nx-1,p+1,expon+1,Prob_distinct*((1.0-stack_location_prob[p])*Prob_miss_shift-1.0)*cur_coefficient,d_init,nx_init);
			PMiss(d-1,nx-1,p,expon+1,Prob_distinct*(1.0-stack_location_prob[p])*(1.0-Prob_miss_shift)*cur_coefficient,d_init,nx_init);
			PMiss(d,nx-1,p+1,expon+1,(1.0-Prob_distinct)*(1.0-stack_location_prob[p])*Prob_miss_shift*cur_coefficient,d_init,nx_init);
			PMiss(d,nx-1,p,expon+1,(1.0-Prob_distinct)*((1.0-stack_location_prob[p])*(1.0-Prob_miss_shift)-1.0)*cur_coefficient,d_init,nx_init);
			PMiss(d,nx-1,p,expon,(1.0-Prob_distinct)*cur_coefficient,d_init,nx_init);
		}

	}

	return; 


}


/**
  * Specifies the filename in which to write the calculated coefficient values
 */
int writeWork(int dx, int nx, char name[20])
{
	int i,j;
	FILE *fp;


	///////////clear  poly_coefficient/////////
	for (i=0;i<mAssoc+1;i++)
		for (j=0;j<MAX_NX;j++)
			poly_coefficient[i][j]=0;
	//////////////////////////////////////
	if (dx>nx)
	{
		printf("ERROR: dx cannot be greater than nx\n");
		printf("Please enter valid values\n");
		exit(1);
		return -1;
	}

	char strFile[200];
	char sdx[200];
	char snx[200];
	char sassoc[200];
	sprintf(sdx,"%d",dx);
	sprintf(snx,"%d",nx);
	sprintf(sassoc,"%d",nassoc);

	switch (NRU)
	{
	case 1:
		
		strcpy(strFile,"./fine/NMRU4/d_");
		strcat(strFile, sdx);
		strcat(strFile, "/NMRU4");
		break;
	case 2:
		
		strcpy(strFile,"./fine/NMRU1/d_");
		strcat(strFile,sdx);
		strcat(strFile,"/NMRU1");

		break;
	case 3:
		
		strcpy(strFile,"./fine/Rand-LRUskw/d_");
		strcat(strFile,sdx);
		strcat(strFile,"/Rand-LRUskw");
		break;
	case 4:
		
		strcpy(strFile,"./fine/Rand-MRUskw/d_");
		strcat(strFile,sdx);
		strcat(strFile,"/Rand-MRUskw");

		break;
	case 5:

		strcpy(strFile,"./fine/");
		strcat(strFile,name);
		strcat(strFile,"/d_");
		strcat(strFile,sdx);
		strcat(strFile,"/");
		strcat(strFile,name);
		break;
	default: break;

	}

	strcat(strFile,":");
	strcat(strFile,sassoc);
	strcat(strFile,"_");
	strcat(strFile,sdx);
	strcat(strFile,"_");
	strcat(strFile,snx);
	strFile[strlen(strFile)] = 0;

	PMiss(dx,nx,0,0,1,dx,nx);


	fp=fopen(strFile,"w");

	for (i=0;i<MAX_NX;i++)
	{
		fprintf(fp,"%f\t",poly_coefficient[0][i]);
	}
	
	fclose(fp);
	return 1;
}
