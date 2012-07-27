/**
  * Please refer to License.txt for License information.
 */
#include "common.h"
#include "contention.h"
#include "changeAssoc.h"
#include "changerp.h"
#include "createrp.h"

void help()
{
	printf("\n******** ACAPP TOOL HELP MENU ********\n");
	printf("General Usage: \n");
	printf("-h  --- HELP MENU\n");
	printf("Prediction under varying cache associativity: \n");
	printf("\t -a <assoc> [<min assoc> <max assoc>] -f1 <profile1> \n");
	printf("Prediction under varying cache replacement policies: \n");
	printf("\t -p <rpindex> -f1 <profile1> \n");
	printf("\t -pA -f1 <profile1> \n");
	printf("Prediction under cache sharing: \n");
        printf("\t -c -f1 <profile1> -f2 <profile2>\n");
	printf("Adding new replacement policy:(require 'usr_rp.in')\n");
	printf("\t -n (default) or \n");
	printf("\t -n <dx> <nxmin> <nxmax>\n");
	printf("Print supported replacement policies\n");
	printf("\t -log\n");



}

/**
  * Calls the functions of the acapp tool according to user-specified input parameters
 */
int main(int argc, char *argv[])
{
	FILE *fpmain;
	char newDir[200];
	char chstr[10];
	char line[200];
	char *conv;
	int x;
	int i,dx,nx_min,nx_max;
	int newnum, readChk;
	int rpexists;
	x = 1;
	rpexists = 0;
	if ((argc <= 1) || (strcmp(argv[x],"-h") == 0))
	{

		help();
	} else if ((argc == 5) && (strcmp(argv[x],"-a") == 0) && (strcmp(argv[x+2],"-f1") == 0) && (argv[x+1] != NULL))
	{
		readCseq(argv[x+3]);

		sumAssoc(argv[x+1],argv[x+1]);

		freeSpace();

	} else if ((argc == 6) && (strcmp(argv[x],"-a") == 0) && (strcmp(argv[x+3],"-f1") == 0) && (argv[x+1] != NULL) && (argv[x+2] != NULL))
	{
		readCseq(argv[x+4]);

		sumAssoc(argv[x+1],argv[x+2]);

		freeSpace();
	} else if ((argc == 5) && (strcmp(argv[x],"-p") == 0) && (argv[x+1] != NULL) && (strcmp(argv[x+2],"-f1") == 0) && (argv[x+3]!= NULL))
	{
		readCseq(argv[x+3]);
		readCseq2(atoi(argv[x+1]));
		printCseq();
		readChk = Work();

		if (readChk != -2)
		{
			Pre_coefficient();
			Report(Newton_Raphson(40, 0.5));

		}

		freeSpace();



	} else if ((argc== 4) && (strcmp(argv[x],"-pA") == 0)  && (strcmp(argv[x+1],"-f1") == 0) && (argv[x+2]!= NULL) )
	{

		readCseq(argv[x+2]);
		printCseq();
		freeSpace();
		if ((fpmain = fopen("rplist.log","r")) == NULL)
		{
			printf("ERROR: Please provide logfile rplist.log in working directory\n");
			exit(1);
		} else
		{
			while (fgets(line,200,fpmain) != NULL)
			{
				readChk = 0;
				readCseq(argv[x+2]);
				printf("\n%s",line);
				conv = strtok(line," ");

				newnum = atoi(conv);    
				readCseq2(newnum);

				readChk = Work();

				if (readChk != -2)
				{
					Pre_coefficient();
					Report(Newton_Raphson(40, 0.5));
					printf("*******************************\n");
				}
				



				freeSpace();

			}
			fclose(fpmain);
		}



	}

	else if ((argc == 6) && (strcmp(argv[x],"-c") == 0) && (strcmp(argv[x+1],"-f1") == 0) && (argv[x+3] != NULL))
	{
		readCseqCon(argv[x+2],argv[x+4]);
		prob(BENCH_MARK1);
		prob(BENCH_MARK2);
		printDetails(argv[x+2],argv[x+4]);
		freeSpace();



	}

	else if ((strcmp(argv[x],"-n") == 0) && ((argc == 2) || (argc == 5)))
	{
		//default
		
		readCseq3(5);
		
		if ((fpmain = fopen("rplist.log","r")) == NULL)
		{
			printf("ERROR: Please provide logfile rplist.log in working directory\n");
			exit(1);
		} else
		{


			while (fgets(line,200,fpmain) != NULL)
			{
				conv = strtok(line," ");
				newnum = atoi(conv);
				conv = strtok(NULL," ");
				conv= strtok(NULL," ");
				conv[strlen(conv)-1]= 0;
				if (strcmp(conv,newrp_name) == 0)
				{
					rpexists = 1;
				}

			}



			newnum++;
			fclose(fpmain);


		}
		
		
		
		
		if ((nassoc && !(nassoc & (nassoc - 1))) != 1)
		{
			printf("ERROR: Please enter associativity in usr_rp.in as power of 2 in input file usr_rp.in \n");
			exit(1);
		}
		/**default case*/
		if (argc < 5)
		{

			if (rpexists == 1)
			{
				printf("ERROR: Cannot add coefficient file(s) since rp %s already exists\n",newrp_name);
				printf("Consider changing replacement policy's name to run default case\n");
				exit(1);
				
				
			
			}
			else {
			
			
			printf("Creating new replacement policy...\n");
			
			strcpy(newDir,"./fine/");
			strcat(newDir,newrp_name);
			mkdir(newDir,0777);

				for (dx=0; dx < nassoc * 3; dx++)
				{
					strcpy(newDir,"./fine/");
					strcat(newDir,newrp_name);
					strcat(newDir,"/d_");
					sprintf(chstr,"%d",dx);
	
					strcat(newDir,chstr);
					printf("\n Coefficient file(s) added to: %s\n",newDir);
					mkdir(newDir,0777);

					for (i=dx;i<=20;i++)
					writeWork(dx,i,newrp_name);
				}   

			

		if ((fpmain = fopen("rplist.log","a")) == NULL)
		{
			printf("ERROR: Please provide logfile rplist.log in working directory\n");
			exit(1);
		} else
		{
			fprintf(fpmain,"%d - %s\n",newnum,newrp_name);
			printf("Replacement policy: '%d - %s' Added Successfully!\n",newnum,newrp_name);
			fclose(fpmain);
		}


		}
			

		}

		//if arguments specified
		else
		{
			
			if (rpexists == 1)
			{
				printf("Adding coefficient file(s) to rp: %s\n",newrp_name);
			}
			else {
			
				printf("Creating new rp: %s\n",newrp_name);
				printf("Adding specified coefficient file(s)...\n");
				
			
			}
	
			dx=atoi(argv[2]);
			nx_min=atoi(argv[3]);
			nx_max=atoi(argv[4]);



			strcpy(newDir,"./fine/");
			strcat(newDir,newrp_name);
			mkdir(newDir,0777);


			strcpy(newDir,"./fine/");
			strcat(newDir,newrp_name);
			strcat(newDir,"/d_");
			sprintf(chstr,"%d",dx);

			strcat(newDir,chstr);
			printf("\nCoefficient file(s) added to directory: %s\n",newDir);
			mkdir(newDir,0777);
			for (i=nx_min;i<=nx_max;i++)
				writeWork(dx,i,newrp_name);
				
			if (rpexists != 1)
			{
				if ((fpmain = fopen("rplist.log","a")) == NULL)
				{
					printf("ERROR: Please provide logfile rplist.log in working directory\n");
					exit(1);
				} else
				{
					fprintf(fpmain,"%d - %s\n",newnum,newrp_name);
					printf("Replacement policy: '%d - %s' Added Succesfully!\n",newnum,newrp_name);
					fclose(fpmain);
				}
			}

			
		}

		freeSpace2();
		
		
	} else if ((strcmp(argv[x],"-log") == 0))
	{
		printf("* * * * SUPPORTED REPLACEMENT POLICIES LOGFILE * * * * \n");
		if ((fpmain = fopen("rplist.log","r")) == NULL)
		{
			printf("ERROR: Please provide logfile rplist.log in working directory\n");
			exit(1);
		} else
		{

			while (fgets(line,200,fpmain) != NULL)
				printf("%s",line);


		}



	}

	else
	{

		printf("ERROR: Incorrect arguments specified\n");
		help();

	}

	return 0;

}
